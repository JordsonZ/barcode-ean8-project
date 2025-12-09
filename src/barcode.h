#ifndef BARCODE_H
#define BARCODE_H

int ean8_check_digit(const char *identifier);
char* ean8_generate_bit_sequence(const char *identifier);
int ean8_decode_bit_sequence(const char *bitseq, char *identifier);
int ean8_decode_digit(const char *bits, int is_left_side);

#endif