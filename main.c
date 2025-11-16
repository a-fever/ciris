#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "png_decode.h"

int main(int argc, char *argv[])
{
	/*
	 * - file passed as argument OK
	 * - file extension checked
	 * - sent to respective decoder
	 * - decoder returns raw pixel data
	 * - raw pixel data can be fed to a mkpalette.c
	 * - palette data can be fed to a export.c
	 * - thee end
	 */

	FILE* fptr = fopen(argv[1], "rb");
	unsigned char* magic_num_buffer = malloc(sizeof(unsigned char) * 10);
	memcpy(magic_num_buffer, fptr, 10);

	unsigned int PNG_MAGIC_NUMBER[8] ={0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A};
	//unsigned int GIF_MAGIC_NUMBER[?] =
	//unsigned int JPEG_MAGIC_NUMBER[?] =

	unsigned int i, check = 0;

	//check png

	for (i = 0; i < 8; i++){
		if (magic_num_buffer[i] == PNG_MAGIC_NUMBER[i]){
			check++;
		}
	}

	if (check++ == 8){
		processPNG(fptr);
	}

	//check extension

















}
