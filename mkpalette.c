#include "./png_decode.h"
#include <stdio.h>

int main(int argc, char * argv[])
{
	int i; unsigned char * arrayPtr;
	arrayPtr = processPNG(argv[1]);
	if (arrayPtr == 0){
	    return -1; // file
	}
	for (i = 0; i < 2; i++){
		printf("\n%.2X", arrayPtr[i]);
	}
}
