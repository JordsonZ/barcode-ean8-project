#ifndef PBM_H
#define PBM_H

typedef struct {
    int width;
    int height;
    int **pixels;
} PBMImage;

PBMImage* pbm_load(const char *filename);
void pbm_free(PBMImage *img);
int pbm_save(const char *filename, PBMImage *img);
PBMImage* pbm_generate(const char *bitseq, int margin, int area_width, int height);
int pbm_detect_barcode(PBMImage *img, int *area_width, int *start_x);
char* pbm_extract_bitseq(PBMImage *img, int area_width, int start_x);

#endif