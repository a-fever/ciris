#include "./png_decode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

	struct color_hex{
            unsigned char R;
            unsigned char G;
            unsigned char B;
            unsigned char A;
        };

        typedef struct color_hex color;

        color *pixelArray = malloc(sizeof(color)*imgSize);

        j = 0;

        for (i = 0; i < 64; i++){
            pixelArray[i].R = arrayPtr[j];
            pixelArray[i].G = arrayPtr[j + 1];
            pixelArray[i].B = arrayPtr[j + 2];
            pixelArray[i].A = 0xFF;
            j = j + 4;                    //alpha doesnt matter but i dont wanna ignore it for some reason. what if it has
        }                           //feelings

	for (i = 0; i < 64; i++){ //this feels really really stupid
            printf("#%.2X%.2X%.2X%.2X\n", pixelArray[i].R, pixelArray[i].G ,pixelArray[i].B, pixelArray[i].A);
        }


	// for (i = 0; i < 256; i++){
 //            printf("%.2X", arrayPtr[i]);
 //            if ((i+1) % 4 == 0){
 //                printf(" ");
 //            }if ((i+1) % 32 == 0){
 //            printf("\n");
 //            }
 //        }

}
