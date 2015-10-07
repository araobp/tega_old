#include <stdint.h>

/*
 * Log file name
 */
#define LOG_FILE "conf.db"

/*
 * Default values 
 */
#define MAX_NODES 4095  // The number of hashes, can be extended to 32767
#define MAX_PAYLOAD 4095 // Cab be extended to 65535
#define MAX_KEY 4095  // Key length
#define MAX_VALUES 4095 // Max length of values in a log entory

/*
 * Cuckoo hashing related
 */
#define MAX_KICKOUT 127 // Max number of kick out

/*
 * Tega KVS address
 */
#define ADDRESS "/var/run/tega.sock" 

/*
KVS log entries

Type 0, overhead per entry: 10 bytes

Header(10)                               Values
---------------------------------------- ----------------------
 1  15  1  15  16   2     6    8    16
[0| h1 |0| h2 |ver|ope|v_type|prev|v_len| value0, value1, ...]\n
                  :<-    16     ->:
                    operations

Type 1, overhead per entry: 8 bytes

Header(8)                                    Key   Values
-------------------------------------------- ----- -----------------------
 1  2    1    12   16   2     6    8    16
[1|XX|k_type|k_len|ver|ope|v_type|prev|v_len| key |  value0, value1, ...]\n
                      :<-    16     ->:
                        operations

*/


#define H1(x) ((x) & 0x7fff)
#define H2(x) ((x) & 0x7fff)
#define K_LEN(x) ((x) & 0x0fff)
#define PREV(x) ((x) & 0x0fff)

#define HEADER_TYPE_MASK (1 << 15) 
#define HEADER_TYPE(x) (((x) & HEADER_TYPE_MASK) >> 15)
#define TYPE0 0
#define TYPE1 1

#define OPERATIONS(ope_v_type, prev) (ope_v_type << 8 | prev) 
#define ATTR_HEADER(k_type, k_len) (0x8000 | (k_type << 12) | k_len)

#define K_TYPE_MASK 0x1000 
#define K_TYPE(x) (((x) & K_TYPE_MASK) >> 12)
#define NORMAL 0
#define STASH  1

#define OPE_V_TYPE(x) (x >> 8)

#define OPE(x) ((x) >> 6)
#define INSERT   0
#define SELECT   1
#define UPDATE   2
#define DELETE   3

// ope_v_type
// |ope(2)|v_type(6)  |
//        |(2)|(1)|(3)|
#define V_TYPE_MASK 0x3f
#define V_TYPE(x) ((x) & V_TYPE_MASK) 
#define V_REPEAT(x) ((x) >> 4)
#define SCALAR 0
#define SET    1
#define LIST   2
#define V_KV(x) (((x) & 0b001000) >> 3)
#define KEYS   0
#define VALUES 1
#define SET_KEYS(ope_v_type) ((ope_v_type) &= ~(1<3))
#define SET_VALUES(ope_v_type) ((ope_v_type) |= (1<3))
#define V_BTYPE(x) ((x) & 0b00000111)
#define B_STRING  0b000
#define B_BOOLEAN 0b001
#define B_INT     0b010
#define B_LONG    0b011
#define B_FLOAT   0b100
#define B_DOUBLE  0b101

typedef struct {
    uint16_t hash1;
    uint16_t hash2;
    uint16_t ver;
    uint16_t operations;
    uint16_t v_len;
} type0;

typedef struct {
    uint16_t attr_header;
    uint16_t ver;
    uint16_t operations;
    uint16_t v_len;
} type1;


/* 
PDU

Header(8)                                    Key   Values
-------------------------------------------- ----- -----------------------
 1  2    1    12   16   2     6    8    16
[X|XX|XXXXXX|k_len|ver|ope|v_type|XXXX|v_len| key |  value0, value1, ...]\n
                      :<-    16     ->:
                        operations

X  Padding
*/


#define RESPONSE_OK          0
#define RESPONSE_NG          1
#define RESPONSE_MEMORY_FULL 2
#define RESPONSE_DB_FULL     3

typedef struct {
    uint16_t k_len;
    uint16_t ver;
    uint16_t operations;
    uint16_t v_len;
} pdu;


/* 
 * CUCKOO HASHING
 */

#define CHECK_EGG_MASK 0x8000
#define CHECK_EGG(x) (((x) & CHECK_EGG_MASK) >> 15)
#define CUCKOO(x) ((x) & 0x7fff)
#define NO_EGG_IN_NEST  0
#define EGG_IN_NEST     1

#define CHECK_STASH_MASK 0x8000
#define CHECK_STASH(x) (((x) & CHECK_STASH_MASK) >> 15)
#define NO_EGGS_IN_STASH  0
#define EGGS_IN_STASH     1

typedef struct egg_in_nest {
    uint16_t another_hash;
    uint16_t next_ver;
    long offset;
} egg_in_nest;

typedef struct egg_in_stash {
    uint16_t k_len;
    uint16_t ver;
    long offset;
    //long next;
    struct egg_in_stash *next_egg;
    char *key;
} egg_in_stash;

