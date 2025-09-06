
   /*
    * THE WORLDS GREATEST PNG PRINTER.
    * BUT ONLY IF ITS NOT INTERLACED.
    * YEAH
    */


#include "./png_decode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct color_hex{
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;
};

typedef struct color_hex color;

color* get_parsed(unsigned int* color_index, unsigned int len)
{
    color* result = malloc(sizeof(color*) * len);
    unsigned int j = 0;
    for (unsigned int i = 0; i < (256*256*256); i++){
        if(color_index[i]){
            result[j].R = i/(256*256);
            result[j].G = i/(256);
            result[j].B = i;
            result[j].A = 0xFF;

            //printf("%.6X is %.2X, %.2X, %.2X.\n", i, result[j].R, result[j].G, result[j].B);
            j++;
        }
    }

    return result;
}

void get_printed(color* pixel, unsigned int len)
{
    for (int i = 0; i < len; i++){
         printf("\033[38;2;%d;%d;%dm██\033[0m", pixel[i].R, pixel[i].G, pixel[i].B);
    }
}

int main(int argc, char *argv[])
{
	int i; int j; unsigned char * arrayPtr;
	arrayPtr = processPNG(argv[1]);

        FILE *image = fopen(argv[1], "rb");

        if(image == NULL){
            printf("IMAGE NOT FOUND. DID YOU TYPE THE CORRECT PATH?\n");
            return 0;
        }

	if (arrayPtr == 0){
	    return -1; // file
	}

	unsigned int imgH = getPNGinfo(image, 'h');
	unsigned int imgW = getPNGinfo(image, 'w');
	unsigned int imgSize = getPNGinfo(image, 'a');

        fclose(image);

        color *pixelArray = malloc(sizeof(color*)*imgSize);

        j = 0;

        for (i = 0; i < imgSize; i++){
            pixelArray[i].R = arrayPtr[j];
            pixelArray[i].G = arrayPtr[j + 1];
            pixelArray[i].B = arrayPtr[j + 2];
            pixelArray[i].A = 0xFF;
            j = j + 4;
            get_printed(&pixelArray[i], 1);
            if ((i + 1) % imgW == 0) printf("\n");
        }

        unsigned int *index = malloc(256*256*256*4);
        memset(index, 0, 256*256*256*4);

        if (index == NULL){
            printf("Mario I fucka up the allocation\n"); //hopefully this never ever prints
            return -1;
        }

        j = 0;

        for (i = 0; i < imgSize; i++){
            j = ((pixelArray[i].B) + (pixelArray[i].G * 256) + (pixelArray[i].R * 256 * 256));
            //printf("%X\n ", j);
            *(index + j) = *(index + j) + 1; //cant do ++ =_=
        }
        j = 0;

        unsigned int total_num_colors = 0;

        for (i = 0; i < (256*256*256); i++){
            if (*(index+i)){
                total_num_colors++;
            }
        }

        //get_printed(get_parsed(index, total_num_colors), total_num_colors);

        printf("\n");

        free(index);

}
