iris: mkpalette.c
	cc mkpalette.c export.c png_decode.c /usr/lib/libdeflate.so -lm -o iris
