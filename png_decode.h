#ifndef PNG_DECODE

#define PNG_DECODE
#include <stdio.h>

unsigned char * processPNG(char *fileLocation);

unsigned int getImgInfo(FILE *img, unsigned char type);

#endif
