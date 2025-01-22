/*
Generate bit planes from pgm image

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

FILE *open_pgm(const char *name, int *width, int *height) {
	if (name[0] == '-' && !name[1])
		name = "/dev/stdin";
	FILE *file = fopen(name, "r");
	if (!file) {
		fprintf(stderr, "could not open \"%s\" file for reading\n", name);
		return 0;
	}
	if ('P' != fgetc(file) || '5' != fgetc(file)) {
		fprintf(stderr, "file \"%s\" not a P5 image\n", name);
		fclose(file);
		return 0;
	}
	int integer[3];
	int byte = fgetc(file);
	if (EOF == byte)
		goto eof;
	for (int i = 0; i < 3; i++) {
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
	if (!(integer[0] && integer[1] && integer[2])) {
		fprintf(stderr, "could not read image file \"%s\"\n", name);
		fclose(file);
		return 0;
	}
	if (integer[2] != 255) {
		fprintf(stderr, "cant read \"%s\", only 8 bit channel supported\n", name);
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
	FILE *pgm = open_pgm(argv[2], &width, &height);
	if (!pgm)
		return 1;
	int pixels = width * height;
	unsigned char *buf = calloc(pixels, 1);
	for (unsigned char *p = buf; p < buf + pixels; ++p)
		*p = fgetc(pgm);
	int signs = pixels / 8;
	int bytes = pixels + signs;
	if (gen) {
		putleb128(bytes);
		for (int plane = 7; plane >= 0; --plane)
			for (unsigned char *p = buf; p < buf + pixels; ++p)
				putbit((*p >> plane) & 1);
		while (signs--)
			putbyte(fgetc(pgm));
	} else {
		if (getleb128() != bytes)
			return 1;
		for (int plane = 7; plane >= 0; --plane)
			for (unsigned char *p = buf; p < buf + pixels; ++p)
				if (getbit() != ((*p >> plane) & 1))
					return 1;
		while (signs--)
			if (getbyte() != fgetc(pgm))
				return 1;
	}
	fclose(pgm);
	free(buf);
	fprintf(stderr, "%s: %s %d bytes\n", argv[0], gen ? "generated" : "verified", gen ? wrote_bytes : read_bytes);
	return 0;
}

