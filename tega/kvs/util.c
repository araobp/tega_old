/*
 * 2014/10/5
 *
 * Input: char *key
 * Output: char **hashed_key, **node
 * Returns 0 (False) when reached the tail of the key. 
 */

#include <stdio.h>
#include <string.h>
#include <tega.h>

char *i_key;
size_t hashed_key_offset;
size_t tail_offset;
short i; 
char c;
char i_hashed_key[256];
char i_node[256];
short has_dot;
short return_tail;

void key2node_init(char *key, char **hashed_key, char **node) {
    i_key = key;
    tail_offset = 0;
    *hashed_key = &i_hashed_key[0];
    *node = &i_node[0];
    has_dot = 0;  // False
    return_tail = 0;  // False
}

short key2node_has_next() {

    /*
    // Initialization
    if (i_key != key) {
        i_key = key;
        tail_offset = 0;
        *hashed_key = &i_hashed_key[0];
        *node = &i_node[0];
        has_dot = 0;  // False
        return_tail = 0;  // False
    }
    */
    
    i = 0;
    if (has_dot) {
        i_hashed_key[hashed_key_offset] = '.';
    }
    if (return_tail == 1) {  // True
        i_node[0] = '\0';
        i_hashed_key[tail_offset] = '\0';
        tail_offset = 0;  // Rewinds to the head position.
        return_tail = 0;  // False
        return 0;
    }
    hashed_key_offset = tail_offset - 1;

    while (1) {
        c = *(i_key+tail_offset);
        switch (c) {
            case '.':  // Found '.'
                i_node[i] = '\0';
                tail_offset++;
                if (has_dot == 0)  // False
                {
                    has_dot = 1;  // True
                    i = 0;
                    hashed_key_offset = tail_offset - 1;
                    break;
                } else {
                    i_hashed_key[hashed_key_offset] = '\0';
                    return 1; 
                }
            case '\0':  // Reached the tail position.
                if (has_dot == 1) {  // If the key has '.', then...
                    i_hashed_key[hashed_key_offset] = '\0';
                    i_node[i] = '\0';
                    //tail_offset = 0;  // Rewinds to the head position.
                    return_tail = 1;
                    return 1;
                } else {
                    i_hashed_key[tail_offset] = '\0';
                    i_node[0] = '\0';
                    tail_offset = 0;  // Rewinds to the head position.
                    return 0;
                }
            default:
                i_node[i] = c;
                i_hashed_key[tail_offset] = c;
                tail_offset++;
                i++;
        } 
    }
}

/*
 * if match found, returns 0, match_begin and match_end.
 * if match not found, returns -1 and match_begin as an insert position.
 */
short binary_search(char *csv, char *search_str, size_t *match_begin, size_t *match_end) {
    size_t csv_len = strlen(csv);
    char *csv_end = csv + csv_len;  // address of '\0' in the csv
    size_t right = csv_len;  // binary search region
    size_t left = 0;  // binary search region
    size_t cursor;
    size_t cursor_split = 0;  // binary search position
    size_t cursor_split_before; 
    char c;
    char key_in_csv[MAX_KEY];
    short i = 0;
    int s_cmp;  // strcmp result
    size_t j;

    #ifdef DEBUG
    if (csv_len > 0) {
        printf("\n");
        printf("%s\n", csv);
        for(j=0;j<csv_len;j++) {
            printf("%d", j%10);
        }
        printf("\n     ");
        for(j=0;j<csv_len;j++) {
            if (j%10 == 0) {
                printf("%d", j/10);
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    #endif
    
    if (right == 0) {  // *csv has no values.
        *match_begin = 0;
        *match_end = 0;
        return -1;
    }
    while (1) {
        cursor_split_before = cursor_split;
        cursor_split = (right+left)/2;  // Split into two
        if (cursor_split == cursor_split_before) { // No match found
            if (s_cmp < 0 && *match_end > 0) {
                *match_begin = *match_end + 2; // ...,key,*...,...
            }  // else condition: ..., (reached the end of csv)
            *match_end = *match_begin;
            return -1;
        }
        cursor = cursor_split;

        // Searches the first ','
        if (cursor != 0) {  // The head position does not have ','
            while (1) {
                    c = *(csv+cursor);
                    if (c == ',') {
                        ++cursor;
                        break;
                    } else {
                        ++cursor;
                    }
                }
        }
        *match_begin = cursor;
       
        // Searches the second ',' enclosing a key 
        *match_end = 0;
        i = 0;
        //while (1) {
        while ((csv+cursor) < csv_end) {  // break if it reaches '\0' 
            c = *(csv+cursor);
            if (c == ',') {
                key_in_csv[i] = '\0';
                *match_end = cursor - 1;
                break;
            } else {
                key_in_csv[i++] = c;
                ++cursor;
            }
        }
        
        // String comparison between the key and the search string
        s_cmp = strcmp(key_in_csv, search_str);
        #ifdef DEBUG
        printf("cursor: %u\n", cursor);
        printf("key_in_csv: %s\n", key_in_csv);
        printf("search_str: %s\n", search_str);
        printf("s_cmp: %d\n", s_cmp);
        printf("\n");
        #endif
        if (s_cmp == 0) {  // The key matches the search string
            return 0;
        } else if(s_cmp > 0) {  // Search left next time
            right = cursor_split;
        } else if(s_cmp < 0) {  // Search right next time
            left = cursor_split;
        }
    }
}

/*
 * Returns -1 if the child was found: insertion failed.
 */
short ins_child(char *values, char *child) {

    char right[MAX_KEY];
    size_t insert_position;
    size_t temp;
    short result;
    char *comma = ",";

    result = binary_search(values, child, &insert_position, &temp);

    if(result == 0) {
        return -1;
    } else {
        strcpy(right, values+insert_position);
        *(values+insert_position) = '\0';
        strcat(values, child);
        strcat(values, comma);
        strcat(values, right);
        return 0;
    }
}

/*
 * Returns -1 if the child was NOT found: deletion failed.
 */
short del_child(char *values, char *child) {

    char right[MAX_KEY];
    size_t match_begin;
    size_t match_end;
    char* p;
    short result;
    char *comma = ",";

    result = binary_search(values, child, &match_begin, &match_end);

    if(result == 0) {
        p = values+match_end+2;
        if (*p != '\0') {
            strcpy(right, p);
        } else {
            *right = '\0';
        }
        *(values+match_begin) = '\0';
        strcat(values, right);
        return 0;
    } else {
        return -1;
    }
}

#ifdef BINARY_SEARCH
void main() {
    char *csv = "aaa,aba,abb,baa,bbc,cde,f,g,xxx,y,zzz,";
    char *abb = "abb";
    size_t match_begin;
    size_t match_end;
    short match_found;
    match_found = binary_search(csv, abb, &match_begin, &match_end);
    printf("match_found: %d, match_begin: %d, match_end: %d\n", match_found, match_begin, match_end);
    char *xxx = "xxx";
    match_found = binary_search(csv, xxx, &match_begin, &match_end);
    printf("match_found: %d, match_begin: %d, match_end: %d\n", match_found, match_begin, match_end);
;
    char *aaa = "aaa";
    match_found = binary_search(csv, aaa, &match_begin, &match_end);
    printf("match_found: %d, match_begin: %d, match_end: %d\n", match_found, match_begin, match_end);
    char *zzz = "zzz";
    match_found = binary_search(csv, zzz, &match_begin, &match_end);
    printf("match_found: %d, match_begin: %d, match_end: %d\n", match_found, match_begin, match_end);
    char *bbb = "bbb";
    match_found = binary_search(csv, bbb, &match_begin, &match_end);
    printf("match_found: %d, match_begin: %d, match_end: %d\n", match_found, match_begin, match_end);
    char *a = "a";
    match_found = binary_search(csv, a, &match_begin, &match_end);
    printf("match_found: %d, match_begin: %d, match_end: %d\n", match_found, match_begin, match_end);
    char *zzzz = "zzzz";
    match_found = binary_search(csv, zzzz, &match_begin, &match_end);
    printf("match_found: %d, match_begin: %d, match_end: %d\n", match_found, match_begin, match_end);
}
#endif
#ifdef INS_DEL
void test(char* new_values, char* child, char test_case) {
    char values[MAX_KEY];
    short result;
    if (new_values != NULL) {
        strcpy(values, new_values);
    }
    switch (test_case) {
        case 'i':
            printf("--- ins_child test\n");
            printf("original values: %s\n", values);
            printf("child: %s\n", child);
            result = ins_child(values, child);
            printf("result: %d\n", result);
            printf("     new values: %s\n", values);
            break;
        case 'd':
            printf("--- del_child test\n");
            printf("original values: %s\n", values);
            printf("child: %s\n", child);
            result = del_child(values, child);
            printf("result: %d\n", result);
            printf("     new values: %s\n", values);
            break;
    }

}
void main() {
    test("aaa,aba,abb,baa,bbc,cde,f,g,xxx,y,zzz,", "a", 'i');
    test(NULL, "zzzz", 'i');
    test(NULL, "baa", 'i');
    test(NULL, "baaa", 'i');
    test(NULL, "baaa", 'd');
    test(NULL, "baaa", 'd');
    test(NULL, "a", 'd');
    test(NULL, "zzzz", 'd');
    test("", "alice", 'i');
    test(NULL, "alice", 'd');
    test(NULL, "alice", 'i');
}
#endif

