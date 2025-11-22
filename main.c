#include <stdio.h>
#include <stdlib.h> //malloc
#include <string.h> //NULL, memcpy

#include "png_decode.h"
//#include "mkpalette.h"


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

	FILE* fptr = fopen("./img/gay.png", "rb");

	if(fptr == NULL){
		printf("IMAGE NOT FOUND. DID YOU TYPE THE CORRECT PATH?\n");
		return -1;
	}

	unsigned char* magic_num_buffer = malloc(sizeof(unsigned char) * 10);
	fread(magic_num_buffer, 1, 10, fptr);

	for (int i = 0; i < 10; i++){
		printf("%.2x", magic_num_buffer[i]);
	}printf("\n");

	unsigned int PNG_MAGIC_NUMBER[8] ={0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A};
	//unsigned int GIF_MAGIC_NUMBER[?] =
	//unsigned int JPEG_MAGIC_NUMBER[?] =

	unsigned int i, check = 0; //index, checking byte array validity
	unsigned int is_valid = 0; //breaks while loop once a file is validated

	unsigned char* raw_pixel_ptr;
	unsigned long raw_length;

	while (!is_valid){
		for (i = 0; i < 8; i++){ // maybe make a function
			if (magic_num_buffer[i] == PNG_MAGIC_NUMBER[i]){
				check++;
			}
		} if (check == 8){
			raw_pixel_ptr = processPNG(fptr);
			raw_length = getPNGinfo(fptr, 'a');
			is_valid = 1; //breaks loop
		} else return 1;
	}

	free(magic_num_buffer);
	return 0;
}
