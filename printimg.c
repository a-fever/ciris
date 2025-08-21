#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "math.h"
#include "/usr/include/libdeflate.h"
int i = 0;
int PNG_MAGIC_NUMBER[8] ={0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A};

	/* TODO:
	* add more checks (what happens if the keyword isnt found in scanner?)
	* implement zlib
	* start splitting program up. we should have a image checker function
	*/

int scanner(char *keyword,unsigned char *target,unsigned long imageSize) //finds the first instance of the keyword
{
	int i, j;
	int location;

	for (i = 0; i < imageSize; i++){
		if (target[i] == keyword[0] && target[i+1] == keyword[1] && target[i+2] == keyword[2] && target[i+3] == keyword[3]){ //THIS IS FUCKED.
			location = i+4;
			i = imageSize;
		}
	}

	if (location == 0){ //FIXME doesnt work???
		printf("SCAN ERROR: \"%s\" WAS NOT FOUND IN THE TARGET STRING.\n", keyword);
		return -1;
	}

	else {
		return location;
	}

}
unsigned int getImgInfo(FILE *img, unsigned char type)
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

	for (i = 0; i < 4; i++){	//combining the bytes
		width = width * 10;
		width = width_b[i] + width;
	}
	for (i = 0; i < 4; i++){
		height = height * 10;
		height = height_b[i] + height;
	} 				//i fucking hate c language the older cpus make me manually make a 4 byte integer and shout go white boy go
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

int main(int argc, char * argv[])
{//image opener and checker
	FILE *image = fopen(argv[1], "rb");

	if(image == NULL){
		printf("IMAGE NOT FOUND. DID YOU TYPE THE CORRECT PATH?\n");
		return -1;
	}

	fseek(image, 0L, SEEK_END);
	unsigned long imageSize = ftell(image);
	rewind(image);

	unsigned char imageData[imageSize];		// TODO: refactor so that imageData[] never gets made. its only
	unsigned char *imageDataPtr = imageData;	// used to look for IDAT/IEND which can be found by
	fread(imageData, 1, imageSize, image);		// reading directly from the file.
	int i = 0;

	while (PNG_MAGIC_NUMBER[i] == imageData[i] && i < 8) {
	i++;
	}
	if (i < 8){
		printf("INVALID OR CORRUPTED PNG.\n");
		return -1;
	}
	int imgDataStart = scanner("IDAT",imageDataPtr,imageSize);
	int imgDataEnd = scanner("IEND", imageDataPtr, imageSize) - 8;

	int IDAT_size = imgDataEnd - imgDataStart;

	unsigned char IDAT_buffer[IDAT_size];

	for (i = 0; i < IDAT_size; i++){
		IDAT_buffer[i] = imageData[imgDataStart+i];
	}
//end of checker

//zlib

	void *decompressor = libdeflate_alloc_decompressor();

	size_t bufferSize = getImgInfo(image, 'a')*5;

	unsigned char buffer[bufferSize];

	size_t *bufferSizeActualPtr = &bufferSize;
	size_t *actualBytesOut;
	int returnCode = libdeflate_zlib_decompress(decompressor, IDAT_buffer, IDAT_size, buffer, bufferSize, bufferSizeActualPtr);
	printf("\n%d\n\n", returnCode);

// end of decompression
// now the fun really begins

	unsigned int imageArray[(getImgInfo(image,'w')*4-1)][(getImgInfo(image,'h'))];

	int j; int k;
			//i is the vertical index, j is the horizontal jndex, and k is the total counter kndex :-)
	for (i = 0; i < getImgInfo(image, 'h'); i++){
		for (j = 0; j < getImgInfo(image, 'w')*4; j++){
		imageArray[j][i] = buffer[k];
		printf("%.2X ", imageArray[j][i]);
		k++;
		}
		printf("\n");
		k++;
	}

	printf("\n");


	for (i = 0; i < getImgInfo(image, 'h'); i++){	// [c][b]
	    switch (imageArray[0][i]) {			// [a][x]
		case 0x00: // x = x
		    for (j = 1; j < getImgInfo(image,'w')*4; j++){
			imageArray[j-1][i] = (imageArray[j][i]);
		    }
		case 0x01: // x =
		    for (j = 0; j < 4; j++){
			imageArray[j][i] = imageArray[j+1][i];
		    }
		    for (j = 4; j <getImgInfo(image, 'h')*4; j++){
			imageArray[j][i] = imageArray[j-4][i];
		    }

		    break;
		case 0x02: // x = x + b
		    for (j = 5; j < getImgInfo(image,'w')*4+1; j++){
			if (i != 0){ //will a first line ever have 0x02? i dont think so lol
			    imageArray[j][i] = imageArray[j - 4][i] + imageArray[j][i];
			}else{
			    break;
			}
		    } break;
		case 0x03: // x = mod256(x + (a + b)/2)
		    imageArray[j][i] = imageArray[j+1][i];
		    imageArray[j][i] = avgFilter(imageArray[j-1][i],imageArray[j][i-1]) + imageArray[j][i];
		case 0x04: // paeth algorithm
		    for (j = 0; j < getImgInfo(image,'w')*4+1; j++){
			imageArray[j][i] = imageArray[j+1][i];
			if (j == 0){
			imageArray[j][i] = mod256(paethPredictor(0, imageArray[j][i-1], 0) + imageArray[j][i]);
			} else if (j != 0 && j < 4){
			imageArray[j][i] = mod256(paethPredictor(0, imageArray[j][i-1], 0) + imageArray[j][i]);
			} else {
			imageArray[j][i] = mod256(paethPredictor(imageArray[j-4][i], imageArray[j][i-1], imageArray[j-4][i-1]) + imageArray[j][i]);
			}
                    }
		default:
		    break;
	    }
	}

	printf("\n");
	for (i = 0; i < getImgInfo(image, 'h'); i++){
	    printf("%d:", i);
	    for (j = 0; j < getImgInfo(image, 'w')*4; j++){
		if (j % 4 == 0){
		    printf(" #");}
		printf("%.2X", imageArray[j][i]);
	    }
	    printf("\n");
	}

	fclose(image);
	return 0;
}
