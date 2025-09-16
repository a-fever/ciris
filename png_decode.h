#ifndef PNG_DECODE

#define PNG_DECODE
#include <stdio.h>

unsigned char * processPNG(char *fileLocation);

unsigned int get_png_size(FILE *img, unsigned char type);

#endif
