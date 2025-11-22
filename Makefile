iris: mkpalette.c
	cc main.c png_decode.c /usr/lib/libdeflate.so -lm -g -o iris
