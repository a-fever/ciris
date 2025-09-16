#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "math.h"
#include "/usr/include/libdeflate.h"

int i = 0;
int PNG_MAGIC_NUMBER[8] ={0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A};

//TODO: replace all the VLAs with malloced arrays
//	clean up testing print functions
//      test larger files, libdef doesnt like it for some reason

// BUG: larger images return non-zero
// cause may be the wrong decompression method (im skeptical)
// or it not knowign what to do with multiple IDAT chunks (likely)

unsigned int scanner(char *keyword,unsigned char *target,unsigned long imageSize) //finds the first instance of the keyword
{
	int i;
	int location = 0;

	for (i = 0; i < imageSize; i++){
		if (target[i] == keyword[0] && target[i+1] == keyword[1] && target[i+2] == keyword[2] && target[i+3] == keyword[3]){
			//printf("%c%c%c%c = %c%c%c%c\n", target[i],target[i+1],target[i+2],target[i+3],keyword[0],keyword[1],keyword[2],keyword[3]);
			location = i+4;
			i = imageSize;
		}
	}

		return location;
}

//better_scanner accepts two keywords (one start and one finish)
// a better method is to have it work with the target via actual
// file functs instead of whatever all this is.
// like let it fscan thru instead of working w the imageData var
// and also thisll let us get rid of imageData for realsies
// because its really really really useless
// i just wasnt aware of how much u can rlly do with the f functions

// TODO: look at the stdlib source code. not important
// im just curious. goodnight cruel world.

typedef struct keyword{
	char* word[5];
} keyword;

unsigned int better_scanner(unsigned char *target,unsigned long imageSize) //WIP
{
	keyword chunk_keywords[2] = {"IDAT", "IEND"};
	return 0;
}

unsigned int get_png_size(FILE *img, unsigned char type)
//returns width, height, or area depending on parameter set
{
	rewind(img);
	fseek(img, 16, SEEK_SET);

	unsigned char width_b[4];	//width in bytes
	unsigned char height_b[4];	//height in bytes

	unsigned int width = 0;
	unsigned int height = 0;

	fread(width_b, 1, 4, img);	//goes through and manually saves each byte to their arrays. endian nonsense that i honestly dont understand
	fread(height_b, 1, 4, img);
	rewind(img);

	for (i = 0; i < 4; i++){	//combining the bytes. theres gotta be a better way of doing this
		width = width * 10;
		width = width_b[i] + width;
	}
	for (i = 0; i < 4; i++){
		height = height * 10;
		height = height_b[i] + height;
	}

	if (type == 'h'){
		return height;
	}if (type == 'w'){
		return width;
	} else return width * height;
}

int mod256(int x)
{
	while (x > 255){
		x = x - 256;
	}
	return x;
}

int avgFilter(float a, float b)
{
    return floor((a+b)/2);
}

int paethPredictor(int a, int b, int c)
{
	int p = a + b - c;
	int pa = abs(p - a);
	int pb = abs(p - b);
	int pc = abs(p - c);

	if (pa <= pb && pa <= pc){
		//printf(" \tA\n");
		return a;
	}
	else if (pb <= pc){
		//printf("\tB\n");
		return b;
	}
	else{
		//printf("\tC\n");
		return c;
	}
}

unsigned char *processPNG(char *fileLoc) //this is maybe the stupidest shit i will ever write.
{//image opener and checker

	FILE *image = fopen(fileLoc, "rb");

	if(image == NULL){
		printf("IMAGE NOT FOUND. DID YOU TYPE THE CORRECT PATH?\n");
		return 0;
	}

	unsigned int imgW = get_png_size(image,'w');
	unsigned int imgH = get_png_size(image,'h');


	fseek(image, 0L, SEEK_END);
	unsigned long imageSize = ftell(image);
	rewind(image);

	unsigned char *imageData = malloc(imageSize*(sizeof(unsigned char)));	// TODO: refactor so that imageData[] never gets made. its only
	// used to look for IDAT/IEND which can be found by
	fread(imageData, 1, imageSize, image);		// reading directly from the file.
	int i = 0;

	while (PNG_MAGIC_NUMBER[i] == imageData[i] && i < 8) {
		i++;
	}
	if (i < 8){
		printf("INVALID OR CORRUPTED PNG.\n");
		return 0;
	}
	int imgDataStart = scanner("IDAT",imageData,imageSize);
	int imgDataEnd = scanner("IEND", imageData, imageSize) - 8;

	int IDAT_size = imgDataEnd - imgDataStart;

	// printf("size: %X, %X\n", imgDataStart, imgDataEnd);
	// printf("size: %lu, %d\n", imageSize, IDAT_size);

	unsigned char* IDAT_buffer = malloc(IDAT_size*(sizeof(unsigned char)));

	for (i = 0; i < IDAT_size; i++){
		IDAT_buffer[i] = imageData[imgDataStart+i];
	}

//libdeflate stuff

	void *decompressor = libdeflate_alloc_decompressor();

	size_t bufferSize = imgW * imgH * 64;

	unsigned char *buffer = malloc(bufferSize*sizeof(unsigned char*));

	size_t *bufferSizeActualPtr = &bufferSize;
	//size_t *actualBytesOut;
	int returnCode = libdeflate_zlib_decompress(decompressor, IDAT_buffer, IDAT_size, buffer, bufferSize, bufferSizeActualPtr);
	printf("\n%d\n\n", returnCode);

// now the fun really begins

	unsigned char *imageArray = malloc (sizeof(unsigned char)*imgW*imgH*5);

	int j = 0; int k = 0;
			//i is the vertical index, j is the horizontal jndex, and k is the total counter kndex :-)
	for (i = 0; i < imgH; i++){
		for (j = 0; j < imgW*4; j++){
			imageArray[j + (i*imgW*4)] = buffer[k];
			k++;
		}
		k++;
	}

	//printf("\n");
	for (i = 0; i < imgH; i++){	                // [c][b]
	    switch (imageArray[i*imgW*4]) {			// [a][x]
		case 0x00: // x = x
			//printf("0");
		    for (j = 1; j < imgW*4; j++){
			imageArray[(j-1) + (i*imgW*4)] = (imageArray[j + (i*imgW*4)]);
		    }break;

		case 0x01: // x = x + a
			//printf("1");
		    for (j = 1; j < 5; j++){
			imageArray[(j-1) + (i*imgW*4)] = imageArray[j + (i*imgW*4)];
		    }
		    for (j = 5; j <imgW*4; j++){
			imageArray[(j-1) + (i*imgW*4)] = mod256(imageArray[(j-5) + (i*imgW*4)] + imageArray[j + (i*imgW*4)]);
		    }

		    break;


		case 0x02: // x = x + b
			//printf("2");
		    for (j = 1; j < imgW*4+1; j++){
			    imageArray[(j-1) + (i*imgW*4)] = mod256(imageArray[j + (i*imgW*4)]+imageArray[(j-1) + ((i-1)*imgW*4)]);
		    }break;


		case 0x03: // x = mod256(x + (a + b)/2)
			//printf("3");
			for (j = 1; j < 5; j++){
				imageArray[(j-1) + (i*imgW*4)] = mod256(avgFilter(0,imageArray[(j-1) + ((i-1)*imgW*4)]) + imageArray[(j) + (i*imgW*4)]);
			}
			for (j = 5; j < imgW*4+1; j++){
				imageArray[(j-1) + (i*imgW*4)] = mod256(avgFilter(imageArray[(j-5) + (i*imgW*4)],imageArray[(j-1) + ((i-1)*imgW*4)]) + imageArray[j + (i*imgW*4)]);
			}
		    break;


			case 0x04: // paeth algorithm
			//printf("4");
		    for (j = 0; j < imgW*4; j++){
			imageArray[j + (i*imgW*4)] = imageArray[(j+1) + (i*imgW*4)];
			if (j == 0){ //if its the first entry, a and c == 0
			imageArray[j + (i*imgW*4)] = mod256(paethPredictor(0, imageArray[j + ((i-1)*imgW*4)], 0) + imageArray[j + (i*imgW*4)]);
			} else if (j != 0 && j < 4){ //if its one of the first 4 bytes, a and c == 0;
			imageArray[j + (i*imgW*4)] = mod256(paethPredictor(0, imageArray[j + ((i-1)*imgW*4)], 0) + imageArray[j + (i*imgW*4)]);
			} else { //else, normal paeth decoding
			imageArray[j + (i*imgW*4)] = mod256(paethPredictor(imageArray[(j-4) + (i*imgW*4)], imageArray[j + ((i-1)*imgW*4)], imageArray[(j-4) + ((i-1)*imgW*4)]) + imageArray[j + (i*imgW*4)]);
			}
                    } break;
		default:
		    break;
	    }
	}

	//my life wouldve been so much easier had i converted the imageArray into a color struct :|
	//...TODO: convert this to a color struct. we wouldnt have to have all those *4s/-4s/-5s everywhere

	// printf("\n");
	unsigned char *new_buffer = malloc(imgH*imgW*5);

	k = 0;

	for (i = 0; i < imgH; i++){		//turning the pixel data into a single string
	    for (j = 0; j < (imgW*4)-1; j++){	//bc i dont understand how to do it otherwise
		new_buffer[k] = imageArray[j + (i*imgW*4)];
		//printf("%3d ",imageArray[j + (i*imgW*4)]);
		k++;
	    }
	    //printf("\n");
	    k++;
	}

	// printf("\n");
	// for (i = 0; i < imgH; i++){
	//     printf("%d:", i);
	//     for (j = 0; j < imgW*4; j++){
	// 	if (j % 4 == 0){
	// 	    printf(" #");}
	// 	    printf("%.2X", imageArray[j + (i*imgW*4)]);
	//     }
	//     printf("\n");
	// }
 //
	// printf("\n");
	// for (i = 0; i < imgH; i++){
	//     printf("%d:", i);
	//     for (j = 0; j < imgW*4; j++){
	// 	if (j % 4 == 0){
	// 	    printf(" #");}
	// 	    printf("%.2X", imageArray[j + (i*imgW*4)]);
	//     }
	//     printf("\n");
	// }

	unsigned char *image_data = new_buffer;

	fclose(image);
	return image_data;
}
