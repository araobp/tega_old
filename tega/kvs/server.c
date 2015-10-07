#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>

#include <tega.h>

#define BUFSIZE (MAX_PAYLOAD + 1)

char readbuf[BUFSIZE];
char buf[BUFSIZE];
char *p_readbuf;
pdu *p_pdu;
uint16_t k_len;
uint16_t ver;
uint8_t ope_v_type;
uint16_t v_len;
char key[BUFSIZE];
char values[BUFSIZE];
uint8_t ope;
uint8_t v_type;
uint8_t v_repeat;
uint8_t v_kv;
uint8_t v_btype;

short has_next;
char *key_to_be_hashed;
char *node;
uint16_t hash1;
uint16_t hash2;

main() {

    FILE *fp;
    int fd, d;
    struct sockaddr_un saun, fsaun;
    int fromlen = sizeof(fsaun);
    int n;

    // Opens a log file
    log_open();

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("KVS not started: unable to open socket");
        exit(1);
    }

    saun.sun_family = AF_UNIX;
    strcpy(saun.sun_path, ADDRESS);

    unlink(ADDRESS);

    if (bind(fd, (struct sockaddr*)&saun, sizeof(saun)) < 0) {
        perror("KVS not started: unable to bind");
        exit(1);
    }


    if (listen(fd, 5) < 0) {
        perror("KVS not started: unable to listen");
        exit(1);
    }


    while (1) {

        if ((d = accept(fd, (struct sockaddr*)&fsaun, &fromlen)) < 0 ) {
           perror("KVS accept failure");
           exit(1);
        }

        while (1) {

            // Read PDU 
            n = read(d, readbuf, sizeof(readbuf));
            #ifdef DEBUG
                printf("pdu_size: %u\n", n);
            #endif
            if (n < 0) {
                perror("KVS read error");
                exit(1);
            } else if (n == 0) {  // Client connection closed
                close(d);
                break;
            }

            // Parses PDU
            p_pdu = (pdu*)readbuf;
             
            k_len = (size_t)ntohs(p_pdu->k_len);
            ver = ntohs(p_pdu->ver);
            ope_v_type = OPE_V_TYPE(ntohs(p_pdu->operations));
            v_len = ntohs(p_pdu->v_len);
            #ifdef DEBUG
                printf("k_len: %u\n", k_len);
                printf("ver: %u\n", ver);
                printf("ope_v_type: %u\n", ope_v_type);
                printf("v_len: %u\n", v_len);
            #endif

            ope = OPE(ope_v_type);
            v_type = V_TYPE(ope_v_type);
            v_repeat = V_REPEAT(v_type);
            v_kv = V_KV(v_type);
            v_btype = V_BTYPE(v_type);
            #ifdef DEBUG
                printf("ope: %u\n", ope);
                printf("v_type: %u\n", v_type);
                printf("v_repeat: %u\n", v_repeat);
                printf("v_kv: %u\n", v_kv);
                printf("v_btype: %u\n", v_btype);
            #endif
            p_readbuf = &readbuf[sizeof(pdu)];

            // key
            strncpy(key, p_readbuf, k_len);
            key[k_len] = '\0'; 
            #ifdef DEBUG
                printf("key: %s\n", key);
            #endif
            p_readbuf +=  sizeof(char) * k_len;

            // values
            strncpy(values, p_readbuf, v_len);
            values[v_len] = '\0';
            #ifdef DEBUG
                printf("values: %s\n", values);
            #endif

            key2node_init(key, &key_to_be_hashed, &node);

            do {

                has_next = key2node_has_next();
                if (has_next == 1) {  // has more nodes
                    SET_KEYS(ope_v_type);
                } else {  // reached the tail node
                    SET_VALUES(ope_v_type);
                    node =  values;
                }

                #ifdef DEBUG
                    printf("key_to_be_hashed: %s, node: %s\n", key_to_be_hashed, node);
                #endif

                switch (ope) {
                    case INSERT:
                        {
                            // Puts an egg in a vacant nest
                            put_egg(key_to_be_hashed, ver, get_current_offset(), &hash1, &hash2);
                            // Appends an log entry to the log file
                            uint8_t prev = 0;  // TODO: impl
                            append_key_to_be_hashed_log_entry(hash1, hash2, ver, ope_v_type, prev, node);
                        }
                        if (has_next == 0) {
                            strcpy(buf, "OK");
                            if (write(d, buf, strlen(buf)) < 0) {
                                perror("KVS write error");
                                exit(1);
                            }
                        }
                        break;
                    case SELECT:
                        {
                            // Gets an egg in the nest
                            type0 header;
                            long offset = get_egg(key_to_be_hashed, ver);
                            #ifdef DEBUG
                            printf("get_egg returned offset: %u\n", offset);
                            #endif
                            long len = get_log_entory(offset, &header, buf);
                            #ifdef DEBUG
                            printf("log entory: %s\n", buf);
                            #endif
                            if (has_next == 0) {
                                if ((n = write(d, buf, len)) < 0) {
                                    perror("KVS write error");
                                    exit(1);
                                }
                            }
                        }
                        break;
                    case UPDATE:
                       break;
                    case DELETE:
                       break;
                }

            } while(has_next);
        }
    }

    close(fd);

    exit(0);
}
