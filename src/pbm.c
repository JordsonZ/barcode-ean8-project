#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pbm.h"

static int **alloc_matrix(int width, int height) {
    int **m = malloc(height * sizeof(int*));
    if (!m) return NULL;

    for (int i = 0; i < height; i++) {
        m[i] = malloc(width * sizeof(int));
        if (!m[i]) return NULL;
    }
    return m;
}

static void free_matrix(int **m, int height) {
    for (int i = 0; i < height; i++)
        free(m[i]);
    free(m);
}

PBMImage* pbm_load(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;

    char magic[3];
    if (!fgets(magic, sizeof(magic), f)) {
        fclose(f);
        return NULL;
    }

    if (strncmp(magic, "P1", 2) != 0) {
        fclose(f);
        return NULL;
    }

    int width, height;
    if (fscanf(f, "%d %d", &width, &height) != 2) {
        fclose(f);
        return NULL;
    }

    PBMImage *img = malloc(sizeof(PBMImage));
    img->width = width;
    img->height = height;
    img->pixels = alloc_matrix(width, height);

    if (!img->pixels) {
        free(img);
        fclose(f);
        return NULL;
    }

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            if (fscanf(f, "%d", &img->pixels[y][x]) != 1) {
                pbm_free(img);
                fclose(f);
                return NULL;
            }

    fclose(f);
    return img;
}

void pbm_free(PBMImage *img) {
    if (!img) return;
    free_matrix(img->pixels, img->height);
    free(img);
}

int pbm_save(const char *filename, PBMImage *img) {
    FILE *f = fopen(filename, "w");
    if (!f) return 0;

    fprintf(f, "P1\n");
    fprintf(f, "%d %d\n", img->width, img->height);

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++)
            fprintf(f, "%d ", img->pixels[y][x]);
        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}

PBMImage* pbm_generate(const char *bitseq, int margin, int area_width, int height) {
    int bitlen = strlen(bitseq);

    int code_width = bitlen * area_width;
    int full_width = code_width + 2 * margin;
    int full_height = height + 2 * margin;

    PBMImage *img = malloc(sizeof(PBMImage));
    if (!img) return NULL;

    img->width = full_width;
    img->height = full_height;
    img->pixels = alloc_matrix(full_width, full_height);

    if (!img->pixels) {
        free(img);
        return NULL;
    }

    for (int y = 0; y < full_height; y++)
        for (int x = 0; x < full_width; x++)
            img->pixels[y][x] = 0;

    for (int i = 0; i < bitlen; i++) {
        int bit = bitseq[i] - '0';
        int start = margin + i * area_width;

        for (int x = start; x < start + area_width; x++)
            for (int y = margin; y < margin + height; y++)
                img->pixels[y][x] = bit;
    }

    return img;
}

int pbm_detect_barcode(PBMImage *img, int *area_width, int *start_x) {

    int mid = img->height / 2;

    int x = 0;
    while (x < img->width && img->pixels[mid][x] == 0) {
        x++;
    }
    if (x >= img->width - 3)
        return 0;

    int w = 0;
    int t = x;
    while (t < img->width && img->pixels[mid][t] == 1) {
        w++;
        t++;
    }

    if (w < 1)
        return 0;

    *area_width = w;
    *start_x = x;

    return 1;
}

char* pbm_extract_bitseq(PBMImage *img, int area_width, int start_x) {

    int bitcount = 67;

    char *seq = malloc(bitcount + 1);
    if (!seq) return NULL;

    int mid = img->height / 2;

    for (int i = 0; i < bitcount; i++) {
        int px = start_x + i * area_width;
        int pixel = img->pixels[mid][px];
        seq[i] = pixel ? '1' : '0';
    }

    seq[67] = '\0';
    return seq;
}