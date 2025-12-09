#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pbm.h"
#include "barcode.h"

void print_usage() {
    printf("Uso: decode <arquivo_pbm>\n");
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Erro: arquivo PBM não informado.\n");
        print_usage();
        return 1;
    }

    char *filename = argv[1];

    PBMImage *img = pbm_load(filename);

    if (!img) {
        printf("Erro: arquivo inválido ou não é um PBM válido.\n");
        return 1;
    }

    int area_width = 0;
    int start_x = 0;

    if (!pbm_detect_barcode(img, &area_width, &start_x)) {
        printf("Código de barras não encontrado na imagem.\n");
        pbm_free(img);
        return 1;
    }

    char *bitseq = pbm_extract_bitseq(img, area_width, start_x);

    if (!bitseq) {
        printf("Erro ao extrair sequência de bits do código.\n");
        pbm_free(img);
        return 1;
    }

    char identifier[9];

    if (!ean8_decode_bit_sequence(bitseq, identifier)) {
        printf("Erro: não foi possível decodificar o identificador.\n");
        free(bitseq);
        pbm_free(img);
        return 1;
    }

    printf("%s\n", identifier);

    free(bitseq);
    pbm_free(img);

    return 0;
}