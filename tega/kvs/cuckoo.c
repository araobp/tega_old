/*
 * Cuckoo hashing 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <fnv.h>

#include<tega.h>

//#define TRUE_HASH_SIZE ((u_int32_t)0x1000)
#define HASH_SIZE (uint32_t)(MAX_NODES + 1)

// Two nests

egg_in_nest nest1[HASH_SIZE];
egg_in_nest nest2[HASH_SIZE];

egg_in_nest egg;

// Stash head
egg_in_stash *head;

uint32_t hash;
uint16_t h;

// Max count
int max_count = 0;

// FNV-1
uint16_t h1(char *key) {
    hash = fnv_32_buf(key, strlen(key), FNV1_32_INIT);
    h = hash % HASH_SIZE;
    return h;
}
// FNV-1a
uint16_t h2(char *key) {
    hash = fnv_32a_buf(key, strlen(key), FNV1_32_INIT);
    h = hash % HASH_SIZE;
    return h;
}

void kicked_out(uint16_t cuckoo, uint16_t another_hash, long offset, uint8_t sw, int count) {
  uint16_t hash1, hash2;
  uint8_t vacancy;

  if (count > MAX_KICKOUT) {
      perror("REACHED MAX KICKOUT");
      exit(1); 
  }
  if (count > max_count) max_count = count;

  switch (sw) {
     case 0: // use h2
        egg = nest2[cuckoo];
        vacancy = (uint8_t)CHECK_EGG(egg.another_hash);
        if (vacancy == EGG_IN_NEST) {
            nest2[cuckoo].another_hash = another_hash | CHECK_EGG_MASK;
            nest2[cuckoo].offset = offset;
            #ifdef DEBUG
            printf("--- KICKED OUT AND FOUND NEW NEST! ---\n");
            printf("nest2[%u]: (%u, %lu)\n", cuckoo, CUCKOO(nest2[cuckoo].another_hash), nest2[cuckoo].offset);
            #endif
            hash1 = cuckoo;
            cuckoo = CUCKOO(egg.another_hash);
            offset = egg.offset;
            kicked_out(cuckoo, hash1, offset, 1, ++count);
        } else {
            nest2[cuckoo].another_hash = another_hash | CHECK_EGG_MASK;
            nest2[cuckoo].offset = offset;
            #ifdef DEBUG
            printf("--- KICKED OUT AND FOUND NEW NEST! ---\n");
            printf("nest2[%u]: (%u, %lu)\n", cuckoo, CUCKOO(nest2[cuckoo].another_hash), nest2[cuckoo].offset);
            #endif
        }
        break;
     case 1: // use h1 
        egg = nest1[cuckoo];
        vacancy = (uint8_t)CHECK_EGG(egg.another_hash);
        if (vacancy == EGG_IN_NEST) {
            nest1[cuckoo].another_hash = another_hash | CHECK_EGG_MASK;
            nest1[cuckoo].offset = offset;
            #ifdef DEBUG
            printf("--- KICKED OUT AND FOUND NEW NEST! ---\n");
            printf("nest1[%u]: (%u, %lu)\n", cuckoo, CUCKOO(nest1[cuckoo].another_hash), nest1[cuckoo].offset);
            #endif
            hash2 = cuckoo;
            cuckoo = CUCKOO(egg.another_hash);
            offset = egg.offset;
            kicked_out(cuckoo, hash2, offset, 0, ++count);
        } else {
            nest1[cuckoo].another_hash = another_hash | CHECK_EGG_MASK;
            nest1[cuckoo].offset = offset;
            #ifdef DEBUG
            printf("--- KICKED OUT AND FOUND NEW NEST! ---\n");
            printf("nest1[%u]: (%u, %lu)\n", cuckoo, CUCKOO(nest1[cuckoo].another_hash), nest1[cuckoo].offset);
            #endif
        }
        break;
  }
}

void *key_ver(char *key, uint16_t ver, char *strbuf) {
    char sver[7] = ":\0\0\0\0\0";  // ':' + Max 65535 + \0
    char *psver = &sver[1];
    #ifdef DEBUG
        printf("key:%s, ver:%u\n", key, ver);
    #endif

    // strbuf=key:ver
    strbuf[0] = '\0';
    sprintf(psver, "%-"PRIu16, ver);
    strcat(strbuf, key);
    strcat(strbuf, sver);
    #ifdef DEBUG
        printf("sver: %s\n", sver);
        printf("key: %s\n", key);
        printf("strbuf: %s\n", strbuf);
    #endif
}

void put_egg(char *key, uint16_t ver, long offset, uint16_t *p_hash1, uint16_t *p_hash2) {
    uint16_t hash1, hash2, another_hash1, another_hash2, cuckoo;
    uint8_t vacancy;
    char strbuf[MAX_KEY+1];
    key_ver(key, ver, &strbuf[0]);  
    hash1 = h1(strbuf);
    another_hash1 = nest1[hash1].another_hash;
    hash2 = h2(strbuf);
    another_hash2 = nest2[hash2].another_hash;
    if (hash2 == another_hash1 && hash1 == another_hash2) {
        perror("DOUBLE HASH COLLISION");
        exit(1);
    }
    vacancy = (uint8_t)(CHECK_EGG(another_hash2) << 1 | CHECK_EGG(another_hash1));
    #ifdef DEBUG
        printf("h1:%u, h2:%u, vacancy:%u\n", hash1, hash2, vacancy);
    #endif
    switch (vacancy) {
        case 0: // both nests are vacant
            nest1[hash1].another_hash = hash2 | CHECK_EGG_MASK;
            nest1[hash1].offset = offset;
            break;
        case 1: // nest2 is vacant
            nest2[hash2].another_hash = hash1 | CHECK_EGG_MASK;
            nest2[hash2].offset = offset;
            break;
        case 2: // nest1 is vacant
            nest1[hash1].another_hash = hash2 | CHECK_EGG_MASK;
            nest1[hash1].offset = offset;
            break;
        case 3: // both nests are occupied
            switch(hash1 & 0x0001) {
                case 0:
                    egg = nest1[hash1];
                    nest1[hash1].another_hash = hash2 | CHECK_EGG_MASK;
                    nest1[hash1].offset = offset;
                    cuckoo = CUCKOO(egg.another_hash);
                    offset = egg.offset;
                    kicked_out(cuckoo, hash1, offset, 0, 0);
                    break;
                case 1:
                    egg = nest2[hash2];
                    nest2[hash2].another_hash = hash1 | CHECK_EGG_MASK;
                    nest2[hash2].offset = offset;
                    cuckoo = CUCKOO(egg.another_hash);
                    offset = egg.offset;
                    kicked_out(cuckoo, hash2, offset, 1, 0);
                    break;
            break;
            }
    }

    // Returns two hash values
    *p_hash1 = hash1;
    *p_hash2 = hash2;

    #ifdef DEBUG
    printf("---\n");
    printf("nest1[%u]: (%u, %lu)\n", hash1, CUCKOO(nest1[hash1].another_hash), nest1[hash1].offset);
    printf("nest2[%u]: (%u, %lu)\n", hash2, CUCKOO(nest2[hash2].another_hash), nest2[hash2].offset);
    printf("max_count: %d\n", max_count);
    #endif
}

/*
 * Returns offset or -1 (not found)
 */
long get_egg(char *key, uint16_t ver, egg_in_nest *egg) {
    uint16_t hash1, hash2;
    char strbuf[MAX_KEY+1];
    key_ver(key, ver, &strbuf[0]);  
    hash1 = h1(strbuf);
    hash2 = h2(strbuf);
    if (nest1[hash1].another_hash == (hash2 | CHECK_EGG_MASK)) {
        return nest1[hash1].offset;
    }
    else if (nest2[hash2].another_hash == (hash1 | CHECK_EGG_MASK)) {
        return nest2[hash2].offset;
    }
    else {
        return -1;  // egg not found
    }
}

/*
void main() {
    char key[] = "a.b.c";
    printf("h1: %d\n", h1(key, 0));
    printf("h1: %d\n", h1(key, 1));
    printf("h1: %d\n", h1(key, 1));
    printf("h1: %d\n", h1(key, 2));
    printf("h1: %d\n", h1(key, 0xffff));
    printf("h2: %d\n", h2(key, 0xffff));
}
*/
