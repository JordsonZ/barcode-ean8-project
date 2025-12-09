#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "barcode.h"
#include "pbm.h"

#define DEFAULT_MARGIN 4
#define DEFAULT_AREA_WIDTH 3
#define DEFAULT_BARCODE_HEIGHT 50
#define DEFAULT_OUTPUT_FILE "barcode.pbm"

void print_usage() {
    printf("Uso: encode <identificador> [margem] [area_width] [altura] [arquivo_saida]\n");
}

int file_exists(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    fclose(f);
    return 1;
}

int ask_overwrite() {
    char c;
    printf("Arquivo já existe. Deseja sobrescrever? (s/n): ");
    scanf(" %c", &c);
    return (c == 's' || c == 'S');
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Erro: identificador não informado.\n");
        print_usage();
        return 1;
    }

    char *identifier = argv[1];

    if (strlen(identifier) != 8) {
        printf("Erro: identificador deve ter exatamente 8 dígitos.\n");
        return 1;
    }

    for (int i = 0; i < 8; i++) {
        if (!isdigit(identifier[i])) {
            printf("Erro: identificador contém caracteres não numéricos.\n");
            return 1;
        }
    }

    if (!ean8_check_digit(identifier)) {
        printf("Erro: dígito verificador inválido.\n");
        return 1;
    }

    int margin      = (argc >= 3) ? atoi(argv[2]) : DEFAULT_MARGIN;
    int area_width  = (argc >= 4) ? atoi(argv[3]) : DEFAULT_AREA_WIDTH;
    int height      = (argc >= 5) ? atoi(argv[4]) : DEFAULT_BARCODE_HEIGHT;

    char *output_file = (argc >= 6) ? argv[5] : DEFAULT_OUTPUT_FILE;

    if (margin < 0 || area_width <= 0 || height <= 0) {
        printf("Erro: valores inválidos para margem, área ou altura.\n");
        return 1;
    }

    if (file_exists(output_file)) {
        if (!ask_overwrite()) {
            printf("Erro: arquivo resultante já existe.\n");
            return 1;
        }
    }

    char *bitseq = ean8_generate_bit_sequence(identifier);
    if (!bitseq) {
        printf("Erro ao gerar código de barras.\n");
        return 1;
    }

    PBMImage *img = pbm_generate(bitseq, margin, area_width, height);
    if (!img) {
        printf("Erro ao criar imagem PBM.\n");
        free(bitseq);
        return 1;
    }

    if (!pbm_save(output_file, img)) {
        printf("Erro ao salvar arquivo PBM.\n");
        pbm_free(img);
        free(bitseq);
        return 1;
    }

    printf("Imagem gerada com sucesso: %s\n", output_file);

    pbm_free(img);
    free(bitseq);

    return 0;
}