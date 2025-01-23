/*
Bit stream of dithered binary image

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

FILE *open_pbm(const char *name, int *width, int *height) {
	if (name[0] == '-' && !name[1])
		name = "/dev/stdin";
	FILE *file = fopen(name, "r");
	if (!file) {
		fprintf(stderr, "could not open \"%s\" file for reading\n", name);
		return 0;
	}
	if ('P' != fgetc(file) || '4' != fgetc(file)) {
		fprintf(stderr, "file \"%s\" not a P4 image\n", name);
		fclose(file);
		return 0;
	}
	int integer[2];
	int byte = fgetc(file);
	if (EOF == byte)
		goto eof;
	for (int i = 0; i < 2; i++) {
		while ('#' == (byte = fgetc(file)))
			while ('\n' != (byte = fgetc(file)))
				if (EOF == byte)
					goto eof;
		while ((byte < '0') || ('9' < byte))
			if (EOF == (byte = fgetc(file)))
				goto eof;
		char str[16];
		for (int n = 0; n < 16; n++) {
			if (('0' <= byte) && (byte <= '9') && n < 15) {
				str[n] = byte;
				if (EOF == (byte = fgetc(file)))
					goto eof;
			} else {
				str[n] = 0;
				break;
			}
		}
		integer[i] = atoi(str);
	}
	if (!(integer[0] && integer[1])) {
		fprintf(stderr, "could not read image file \"%s\"\n", name);
		fclose(file);
		return 0;
	}
	*width = integer[0];
	*height = integer[1];
	return file;
eof:
	fclose(file);
	fprintf(stderr, "EOF while reading from \"%s\"\n", name);
	return 0;
}

int main(int argc, char **argv) {
	if (argc != 3)
		return 1;
	if (*argv[1] != 'g' && *argv[1] != 'v')
		return 1;
	int gen = *argv[1] == 'g';
	int width, height;
	FILE *pbm = open_pbm(argv[2], &width, &height);
	if (!pbm)
		return 1;
	int pixels = width * height;
	int bytes = pixels / 8;
	if (gen) {
		putleb128(bytes);
		while (bytes--)
			for (int i = 7, b = fgetc(pbm); i >= 0; --i)
				putbit((b >> i) & 1);
	} else {
		if (getleb128() != bytes)
			return 1;
		while (bytes--)
			for (int i = 7, b = fgetc(pbm); i >= 0; --i)
				if (getbit() != ((b >> i) & 1))
					return 1;
	}
	fclose(pbm);
	fprintf(stderr, "%s: %s %d bytes\n", argv[0], gen ? "generated" : "verified", gen ? wrote_bytes : read_bytes);
	return 0;
}

