/*
 * File: ubx.c
 * Purpose: Structs, defines, and functions used for UBX messages to and from the GPS
 */

#include "ubx.h"
#include "string.h"

/*
 * Calculate checksum of data for UBX, 2 chars are put into buf
 * This is 8-bit Fletcher Algorithm
 */
void UBX_checksum(char *buf, char* data) {
    char ck_a = 0, ck_b = 0;
    uint32_t len = strlen(data);

    for (int i = 0; i < len; i++) {
        ck_a += data[i];
        ck_b += ck_a;
    }
}
