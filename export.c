#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

unsigned char* CONVERT_TO_BYTES(unsigned char width, unsigned char height)
{
	unsigned char* bytes = malloc(sizeof(unsigned char)*8);
	bytes[0] = (width >> 24) & 0xFF;
	bytes[1] = (width >> 16) & 0xFF;
	bytes[2] = (width >> 8) & 0xFF;
	bytes[3] = width & 0xFF;

	bytes[4] = (height >> 24) & 0xFF;
	bytes[5] = (height >> 16) & 0xFF;
	bytes[6] = (height >> 8) & 0xFF;
	bytes[7] = height & 0xFF;

	return bytes;
}

void export_test_png(unsigned char* raw_data, int width, int height)
{

	unsigned char header[16] = {0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A,0,0,0,0x0D,0x49,0x48,0x44,0x52};
	unsigned char* size = CONVERT_TO_BYTES(width, height);
	unsigned char whatever[5] = {0x08,0x06,0,0,0};
	unsigned long raw_data_len = (sizeof(unsigned char[4])*(width*height));

	FILE* png = fopen("./test.png","wb");
	fwrite(header, 1, 16, png);
	fwrite(size, 1, 8, png);
	fwrite(whatever, 1, 5, png);
	fputs("0000", png);
	fputs("IDAT",png);
	fwrite(raw_data, 1, raw_data_len, png);
	fputs("IEND", png);

	fclose(png);
}
