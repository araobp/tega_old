#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tega.h>

FILE *file = NULL;
const char *nl = "\n";
const size_t len_nl = 1;

pdu *p_pdu;
char *p_readbuf;
uint16_t k_len; 
//char key_values[4096]; // TODO: max number should be declared at tega.h

void log_open() {
    if ((file = fopen(LOG_FILE, "wb+")) == NULL) {
        perror("Unable to open log file");
        exit(1);
    }
    setvbuf(file, NULL, _IONBF, 0);
}

void log_close() {
    if (fclose(file) < 0) {
        perror("Unable to close log file");
        exit(1);
    }
}

long get_current_offset() {
    return ftell(file);
}

// Reads Type0 log entory
uint16_t get_log_entory(long offset, type0 *header, char *strbuf) {
    uint16_t v_len;
    long current_offset = get_current_offset();
    size_t size;
    if (file == NULL) {
        return -1;
    } else {
        fseek(file, offset, 0);
        if ((size = fread(header, sizeof(type0), 1, file)) < 0) {
            perror("file read error");
        }
        v_len = header->v_len;
        if ((size = fread(strbuf, sizeof(char), (size_t)v_len, file)) < 0) {
            perror("file read error");
        }
        strbuf[v_len] = '\0';
        fseek(file, current_offset, 0);  // Moves the offset to the end
        return v_len;
    }
}

void append_key_to_be_hashed_log_entry(uint16_t hash1, uint16_t hash2, uint16_t ver, uint8_t ope_v_type, uint8_t prev, char* node) {
    
    type0 header;
    header.hash1 = hash1;
    header.hash2 = hash2;
    header.operations = OPERATIONS(ope_v_type, prev); 
    uint16_t v_len = strlen(node);
    header.v_len = v_len;

    // Appends the log entry to the log file
    fwrite((char *)&header, sizeof(type0), 1, file);
    fwrite(node, (size_t)(v_len), 1, file);
    fwrite(nl, len_nl, 1, file);
}

// deprecated
/*
void append_type1_log_entry(char* readbuf) {
    
    type1 header; 
    pdu *p_pdu = (pdu*)readbuf;
    uint16_t k_len = ntohs(p_pdu->k_len);
    uint16_t v_len;

    // Type 1 header
    header.attr_header = ATTR_HEADER(NORMAL, k_len);
    header.ver = ntohs(p_pdu->ver);
    header.operations = OPERATIONS(ntohs(p_pdu->operations));
    v_len = ntohs(p_pdu->v_len);
    header.v_len = v_len;

    // Type 1 payload
    p_readbuf = &readbuf[sizeof(pdu)];
    
    // Appends the log entry to the log file
    fwrite((char *)&header, sizeof(type1), 1, file);
    fwrite(p_readbuf, (size_t)(k_len+v_len), 1, file);
    fwrite(nl, len_nl, 1, file);
}
*/

void write_type0(type0 *header, char *values) {
    fwrite((char *)header, sizeof(type0), 1, file);
    fwrite(values, strlen(values), 1, file);
    fwrite(nl, len_nl, 1, file);
}

void write_type1(type1 *header, char *key_values) {
    fwrite((char *)header, sizeof(type1), 1, file);
    fwrite(key_values, strlen(key_values), 1, file);
    fwrite(nl, len_nl, 1, file);
}

/*
main() {
    int i;
    log_open();

    type0 header0;
    for (i=1; i<=10; ++i) {
        header0.hash1 = i & TYPE0 ;
        header0.hash2 = i+1;
        header0.ver = i+2;
        header0.operations = 4096 + i;
        char values[] = "root.a.b.1.'2'.{id:1}.x\n\0";
        header0.v_len = strlen(values);
        write_type0(&header0, values, file);
    }

    type1 header1;
    for (i=1; i<=10; ++i) {
        header1.attr_header = i+0xf & TYPE1;
        header1.ver = i+2+0xf;
        header1.operations = 4096 + i +0xf;
        char values[] = "root.a.b.9.'8'.{id:5}.y\n\0";
        header1.v_len = strlen(values);
        write_type1(&header1, values, file);
    }

    log_close();
}
*/
