#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "/usr/include/libdeflate.h"
#include <stdbool.h>

int i = 0;
int PNG_MAGIC_NUMBER[8] ={0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A};

//TODO: replace all the VLAs with malloced arrays DONE
//	clean up testing print functions
//      test larger files, libdef doesnt like it for some reason

unsigned int scanner(char *keyword,unsigned char *target,unsigned long imageSize) //finds the first instance of the keyword
{
	int i;
	int location = 0;

	for (i = 0; i < imageSize; i++){
		if (target[i] == keyword[0] && target[i+1] == keyword[1] && target[i+2] == keyword[2] && target[i+3] == keyword[3]){
			printf("%c%c%c%c = %c%c%c%c\n", target[i],target[i+1],target[i+2],target[i+3],keyword[0],keyword[1],keyword[2],keyword[3]);
			location = i+4;
			i = imageSize;
		}
	}

		return location;
}

//returns width, height, or area depending on parameter set
unsigned int getPNGinfo(FILE *img, unsigned char type)
{
	rewind(img);
	fseek(img, 16, SEEK_SET);

	unsigned char width_b[4];	//width in bytes
	unsigned char height_b[4];	//height in bytes

	unsigned int width = 0;
	unsigned int height = 0;

	fread(width_b, 1, 4, img);
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

unsigned char* DECODE_IDAT(unsigned char* chunk_ptr, unsigned long int len)
{
	unsigned char* decoded;
	return decoded;
}

unsigned long byte_to_num(unsigned char* byte_array)
{
	unsigned long result = 0;
	for (int i = 0; i < 4; i++){
		result = result * 256;
		result = result + byte_array[i];
	}
	return result;
}

unsigned int CHECK_FOR_IEND()
{
	return 0;
}

unsigned char *processPNG(FILE* fptr)
{
	unsigned int imgW = getPNGinfo(fptr, 'w');
	unsigned int imgH = getPNGinfo(fptr, 'h');

	unsigned int i, check = 0; //i check, too!
	unsigned char IDAT[4] = "IDAT";
	unsigned char IEND[4] = "IEND";
	unsigned char* check_buffer = malloc(sizeof(unsigned char)*4);

	fread(check_buffer, 1, 4, fptr);

	for (i = 0; i < 4; i++){ //checks for first IDAT
		if (IDAT[i] == check_buffer[i]){
			check++;
		} else {
			check = 0;
			i = 0;
			fseek(fptr, -3, SEEK_CUR);
			fread(check_buffer, 1, 4, fptr);
		}
	}

	unsigned char* size_in_bytes = malloc(sizeof(unsigned char)*4);
	unsigned long idat_size;
	unsigned long idat_size_sum = 0;

	unsigned char* idat_data_buf = malloc(idat_size*5);
	unsigned char* decomp_result_buf = malloc(idat_size*5);
	unsigned char* concat_idat = malloc(imgW*imgH*5);
	unsigned int reached_iend = false;
	unsigned int ret_val;
	check = 0;
	void* decomp = libdeflate_alloc_decompressor();
	unsigned long* actual_out;

	fseek(fptr, -8, SEEK_CUR); //jump back to size
	while (!reached_iend){ //starts at size of next buffer;
		fread(size_in_bytes, 1, 4, fptr);
		for (i = 0; i < 4; i++){
			printf("%.2x ", size_in_bytes[i]);
		} printf("\n");
		idat_size = byte_to_num(size_in_bytes);

		fseek(fptr, 4, SEEK_CUR); //skip IDAT

		fread(idat_data_buf, 1, idat_size, fptr);
		ret_val = libdeflate_zlib_decompress(decomp, idat_data_buf, idat_size, decomp_result_buf, (idat_size*5), actual_out);
		printf("\n%d\n", ret_val);
		printf("\n%lu\n", idat_size);


		for (i = 0; i < idat_size; i++){
			concat_idat[idat_size_sum + i] = decomp_result_buf[i];
		} idat_size_sum += idat_size;

		fseek(fptr, 8, SEEK_CUR); //skip CRC + size

		fread(check_buffer, 1, 4, fptr);
		for (i = 0; i < 4; i++){ //check for IEND
			if (IEND[i] == check_buffer[i]){
				check++;
			} else {
				i = 4; //break loop. hoe
			}
		}
		if (check == 4){
			reached_iend = true;
		} else {
			fseek(fptr, -1*check, SEEK_CUR); // backup
			check = 0;
			i = 4; //break if loop;
			fseek(fptr, -8, SEEK_CUR); // back to size
		}
	}

	int j = 0; int k = 0;
	// unsigned char* imageArray = malloc(imgW*imgH*5);
 //
	// for (i = 0; i < imgH; i++){
	// 	for (j = 0; j < imgW*4; j++){
	// 		imageArray[j + (i*imgW*4)] = concat_idat[k];
	// 		printf("%.2X ", imageArray[j + (i*imgW*4)]);
	// 		k++;
	// 	}
	// 	printf("\n");
	// 	k++;
	// }
			//i is the vertical index, j is the horizontal jndex, and k is the total counter kndex :-)
	for (i = 0; i < imgH; i++){	                // [c][b]
	    switch (concat_idat[i*imgW*4]) {		// [a][x]
		case 0x00: // x = x
			printf("0");
		    for (j = 1; j <= imgW*4; j++){
			concat_idat[(j-1) + (i*imgW*4)] = (concat_idat[j + (i*imgW*4)]);
		    }break;

		case 0x01: // x = x + a
			printf("1");
		    for (j = 1; j < 5; j++){
			concat_idat[(j-1) + (i*imgW*4)] = concat_idat[j + (i*imgW*4)];
		    }
		    for (j = 5; j <= imgW*4; j++){
			concat_idat[(j-1) + (i*imgW*4)] = mod256(concat_idat[(j-5) + (i*imgW*4)] + concat_idat[j + (i*imgW*4)]);
		    }

		    break;


		case 0x02: // x = x + b
			printf("2");
		    for (j = 1; j < imgW*4+1; j++){
			    concat_idat[(j-1) + (i*imgW*4)] = mod256(concat_idat[j + (i*imgW*4)]+concat_idat[(j-1) + ((i-1)*imgW*4)]);
		    }break;


		case 0x03: // x = mod256(x + (a + b)/2)
			printf("3");
			for (j = 1; j < 5; j++){
				concat_idat[(j-1) + (i*imgW*4)] = mod256(avgFilter(0,concat_idat[(j-1) + ((i-1)*imgW*4)]) + concat_idat[(j) + (i*imgW*4)]);
			}
			for (j = 5; j < imgW*4+1; j++){
				concat_idat[(j-1) + (i*imgW*4)] = mod256(avgFilter(concat_idat[(j-5) + (i*imgW*4)],concat_idat[(j-1) + ((i-1)*imgW*4)]) + concat_idat[j + (i*imgW*4)]);
			}
		    break;


		case 0x04: // paeth algorithm
			printf("4");
		    for (j = 0; j < imgW*4; j++){
			concat_idat[j + (i*imgW*4)] = concat_idat[(j+1) + (i*imgW*4)];
			if (j == 0){ //if its the first entry, a and c == 0
			concat_idat[j + (i*imgW*4)] = mod256(paethPredictor(0, concat_idat[j + ((i-1)*imgW*4)], 0) + concat_idat[j + (i*imgW*4)]);
			} else if (j != 0 && j < 4){ //if its one of the first 4 bytes, a and c == 0;
			concat_idat[j + (i*imgW*4)] = mod256(paethPredictor(0, concat_idat[j + ((i-1)*imgW*4)], 0) + concat_idat[j + (i*imgW*4)]);
			} else { //else, normal paeth decoding
			concat_idat[j + (i*imgW*4)] = mod256(paethPredictor(concat_idat[(j-4) + (i*imgW*4)], concat_idat[j + ((i-1)*imgW*4)], concat_idat[(j-4) + ((i-1)*imgW*4)]) + concat_idat[j + (i*imgW*4)]);
			}
                    } break;
		default:
		    break;
	    }
	}

	//my life wouldve been so much easier had i converted the imageArray into a color struct :|
	//...TODO: convert this to a color struct. we wouldnt have to have all those *4s/-4s/-5s everywhere

	printf("\n");
	unsigned char *new_buffer = malloc(imgH*imgW*5);

	k = 0;

	for (i = 0; i < imgH; i++){		//turning the pixel data into a single string
	    for (j = 0; j < (imgW*4)-1; j++){	//bc i dont understand how to do it otherwise
		new_buffer[k] = concat_idat[j + (i*imgW*4)];
		//printf("%3d ",concat_idat[j + (i*imgW*4)]);
		k++;
	    }
	    //printf("\n");
	    k++;
	}

	printf("\n");
	for (i = 0; i < imgH; i++){
	    printf("%d:", i);
	    for (j = 0; j < imgW*4; j++){
		if (j % 4 == 0){
		    printf(" #");}
		    printf("%.2X", concat_idat[j + (i*imgW*4)]);
	    }
	    printf("\n");
	}

	printf("\n");
	for (i = 0; i < imgH; i++){
	    printf("%d:", i);
	    for (j = 0; j < imgW*4; j++){
		if (j % 4 == 0){
		    printf(" #");}
		    printf("%.2X", concat_idat[j + (i*imgW*4)]);
	    }
	    printf("\n");
	}

	unsigned char *image_data = new_buffer;

	fclose(fptr);
	return image_data;
}
