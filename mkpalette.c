#include "./png_decode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//NOTE: we should probably change this to like. parse.c and make an actual mkpalette.c ... we'll "C"...

struct color_hex{ //TODO: add a hex code entry for the love of god
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;
    unsigned int hex;
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
            result[j].hex = i;

            j++;
        }
    }

    return result;
}

void pixel_print(color pixel)
{
     printf("\033[38;2;%d;%d;%dm██\033[0m", pixel.R, pixel.G, pixel.B);
}

void get_printed(color* pixel, unsigned int len)
{
    for (int i = 0; i < len; i++){
         printf("\033[38;2;%d;%d;%dm██\033[0m", pixel[i].R, pixel[i].G, pixel[i].B);
    }
}

color saturate_color(color pixel)
{
    if (pixel.R < pixel.G && pixel.R < pixel.B){
        pixel.R = 0;
        pixel.G += 20;
        pixel.B += 20;
    } else if (pixel.G < pixel.B){
        pixel.G = 0;
    } else {
        pixel.B = 0;
    }

    return pixel;
}

int main(int argc, char *argv[])
{
	int i; int j; 
	unsigned char * arrayPtr;
	arrayPtr = processPNG(argv[1]);

        FILE *image = fopen(argv[1], "rb");

        if(image == NULL){
            printf("IMAGE NOT FOUND. DID YOU TYPE THE CORRECT PATH?\n");
            return 0;
        }

	if (arrayPtr == 0){
	    return -1; // file
	}

	unsigned int imgH = get_png_size(image, 'h');
	unsigned int imgW = get_png_size(image, 'w');
	unsigned int imgSize = get_png_size(image, 'a');

        fclose(image);


        int palSize = 10;

        color *pixelArray = malloc(sizeof(color*)*imgSize);
        color *palette = malloc (sizeof(color*)*palSize);

        j = 0;

        for (i = 0; i < imgSize; i++){
            pixelArray[i].R = arrayPtr[j];
            pixelArray[i].G = arrayPtr[j + 1];
            pixelArray[i].B = arrayPtr[j + 2];
            pixelArray[i].A = 0xFF;
            j += 4;
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
            *(index + j) += 1;
        }
        j = 0;

        unsigned int total_num_colors = 0;

        for (i = 0; i < (256*256*256); i++){
            if (*(index+i)){
                total_num_colors++;
            }
        }

        // PALETTE STUFF IDGAF ABT RN BECAUSE THIS DOESNT WORK
        // if (total_num_colors < 8){
        //     printf("This image contains too few colors to make a palette. Tough.\n");
        //     return -1;
        // }
        //
        // printf("\n");
        // color* colorParsed = get_parsed(index, total_num_colors);
        // int step = (total_num_colors) / 8;
        //
        // for (i = 0; i <total_num_colors; i += step){
        //     pixel_print(saturate_color(colorParsed[i]));
        //     printf(" - #%.2X%.2X%.2X\n",
        //            colorParsed[i].R,
        //            colorParsed[i].G,
        //            colorParsed[i].B);
        // }
        //
        // printf("\n");
        //
        // free(index);
        //
        // printf("Lightest color: #%X ",
        //     colorParsed[total_num_colors-1].hex);
        //
        // get_printed(&colorParsed[total_num_colors-1],1);
        //
        // color test;
        //
        // test = colorParsed[total_num_colors-1];
        //
        // test = saturate_color(test);
        //
        // //pixel_print(test);
        //
        // printf("\n");
        // printf("Darkest color:  #%X ",
        //        colorParsed[0].hex);
        //
        // get_printed(&colorParsed[0],1);

        printf("\n");



}
