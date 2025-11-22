#ifndef PNG_DECODE

#define PNG_DECODE
#include <stdio.h>

unsigned char * processPNG(FILE* file_pointer);

unsigned int getPNGinfo(FILE *img, unsigned char type);

#endif
