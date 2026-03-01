#include "screenshot.h"
#include "doomgeneric.h"
#include <stdio.h>
#include <time.h>

struct color_t {
    uint32_t b : 8;
    uint32_t g : 8;
    uint32_t r : 8;
    uint32_t a : 8;
};

extern char *output_buffer;
extern size_t output_buffer_size;

static void writePPM(const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (!f) return;
    
    fprintf(f, "P6\n%d %d\n255\n", DOOMGENERIC_RESX, DOOMGENERIC_RESY);
    
    struct color_t *pixel = (struct color_t *)DG_ScreenBuffer;
    for (int i = 0; i < DOOMGENERIC_RESX * DOOMGENERIC_RESY; i++) {
        fputc(pixel->r, f);
        fputc(pixel->g, f);
        fputc(pixel->b, f);
        pixel++;
    }
    fclose(f);
}

static void writeASCII(const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f) return;
    
    // Write the current output_buffer (already contains the rendered ASCII frame)
    fprintf(f, "%s", output_buffer);
    fclose(f);
}

void DG_Screenshot(const char *basename)
{
    char ppm_filename[256];
    char txt_filename[256];
    
    snprintf(ppm_filename, sizeof(ppm_filename), "capturas/%s.ppm", basename);
    snprintf(txt_filename, sizeof(txt_filename), "capturas/%s.txt", basename);
    
    writePPM(ppm_filename);
    writeASCII(txt_filename);
    
    //printf("Screenshots saved: %s, %s\n", ppm_filename, txt_filename);
}