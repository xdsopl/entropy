/*
Byte based run length encoding of a stream of bits

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

void encode(int bytes) {
	if (!bytes--)
		return;
	int prev = getbyte(), count = 0;
	while (bytes--) {
		int byte = getbyte();
		if (prev == byte && count < 255) {
			++count;
		} else {
			putbyte(prev);
			if (prev == 0 || prev == 255)
				putbyte(count);
			else
				while (count--)
					putbyte(prev);
			count = 0;
		}
		prev = byte;
	}
	putbyte(prev);
	if (prev == 0 || prev == 255)
		putbyte(count);
	else
		while (count--)
			putbyte(prev);
}

void decode(int bytes) {
	while (bytes--) {
		int byte = getbyte();
		putbyte(byte);
		if (byte == 0 || byte == 255)
			for (int i = getbyte(); i; --i, --bytes)
				putbyte(byte);
	}
}

int main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	if (*argv[1] != 'e' && *argv[1] != 'd')
		return 1;
	int enc = *argv[1] == 'e';
	int bytes = getleb128();
	if (bytes <= 0)
		return 1;
	putleb128(bytes);
	if (enc)
		encode(bytes);
	else
		decode(bytes);
	double change = 100.0 * (wrote_bytes - read_bytes) / read_bytes;
	fprintf(stderr, "%s: %s %d to %d bytes %+.2f%%\n", argv[0], enc ? "encoded" : "decoded", read_bytes, wrote_bytes, change);
	return 0;
}

