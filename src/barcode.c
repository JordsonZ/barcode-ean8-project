#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "barcode.h"

static const char *L_CODE[10] = {
    "0001101", "0011001", "0010011", "0111101", "0100011",
    "0110001", "0101111", "0111011", "0110111", "0001011"
};

static const char *R_CODE[10] = {
    "1110010", "1100110", "1101100", "1000010", "1011100",
    "1001110", "1010000", "1000100", "1001000", "1110100"
};

int ean8_check_digit(const char *identifier) {
    int sum = 0;

    for (int i = 0; i < 7; i++) {
        int digit = identifier[i] - '0';
        if ((i + 1) % 2 == 1)
            sum += 3 * digit;
        else
            sum += 1 * digit;
    }

    int next10 = ((sum + 9) / 10) * 10;
    int check = next10 - sum;

    int provided = identifier[7] - '0';
    return (check == provided);
}

char* ean8_generate_bit_sequence(const char *identifier) {
    char *bits = malloc(68);
    if (!bits) return NULL;

    bits[0] = '\0';

    strcat(bits, "101");

    for (int i = 0; i < 4; i++) {
        int d = identifier[i] - '0';
        strcat(bits, L_CODE[d]);
    }

    strcat(bits, "01010");

    for (int i = 4; i < 8; i++) {
        int d = identifier[i] - '0';
        strcat(bits, R_CODE[d]);
    }

    strcat(bits, "101");

    return bits;
}

int ean8_decode_digit(const char *bits, int is_left_side) {
    for (int d = 0; d < 10; d++) {
        if (is_left_side) {
            if (strncmp(bits, L_CODE[d], 7) == 0)
                return d;
        } else {
            if (strncmp(bits, R_CODE[d], 7) == 0)
                return d;
        }
    }
    return -1;
}

int ean8_decode_bit_sequence(const char *bitseq, char *identifier) {
    if (strlen(bitseq) != 67) {
        return 0;
    }

    if (strncmp(bitseq, "101", 3) != 0) return 0;
    if (strncmp(bitseq + 3 + 28, "01010", 5) != 0) return 0;
    if (strncmp(bitseq + 3 + 28 + 5 + 28, "101", 3) != 0) return 0;

    int pos = 3;

    for (int i = 0; i < 4; i++) {
        int d = ean8_decode_digit(bitseq + pos, 1);
        if (d < 0) return 0;
        identifier[i] = d + '0';
        pos += 7;
    }

    pos += 5;

    for (int i = 4; i < 8; i++) {
        int d = ean8_decode_digit(bitseq + pos, 0);
        if (d < 0) return 0;
        identifier[i] = d + '0';
        pos += 7;
    }

    identifier[8] = '\0';

    if (!ean8_check_digit(identifier)) {
        return 0;
    }

    return 1;
}