#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fnv.h>

#define TRUE_HASH_SIZE ((u_int32_t)0x1000)

#define BUFSIZE 80

uint32_t hash;
uint16_t h1;
uint16_t h2;

int main() {
    //char data[] = "idb.root1.a.b(id=1).0";
    char data[BUFSIZE];
    FILE *file;
    file = fopen("keys.txt", "r");
    if (file  == NULL) {
	printf("error");
	return (1);
    }
    while (fgets(data, sizeof(data), file) != NULL) {
        // h1(x)
        hash = fnv_32_buf(data, strlen(data), FNV1_32_INIT);
        h1 = hash % TRUE_HASH_SIZE;
        // h2(x)
        hash = fnv_32a_buf(data, strlen(data), FNV1_32_INIT);
        h2 = hash % TRUE_HASH_SIZE;

	printf("%s", data);
        printf("   h1(x): %x\n", h1);
        printf("   h2(x): %x\n", h2);
    }
    return (0);
}
