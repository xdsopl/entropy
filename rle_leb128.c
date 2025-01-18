/*
Byte based run length encoding of a stream of bits

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

int main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	if (*argv[1] != 'e' && *argv[1] != 'd')
		return 1;
	int bytes = getleb128();
	if (bytes <= 0)
		return 1;
	putleb128(bytes);
	if (*argv[1] == 'e') {
		fprintf(stderr, "%s: run length encoding %d bytes\n", argv[0], bytes);
		for (int j = bytes, last = -1, count = -1; j; --j) {
			int byte = getbyte();
			if (byte == 0 || byte == 255) {
				if (last == byte) {
					++count;
				} else if (count < 0) {
					putbyte(byte);
					count = 0;
					last = byte;
				} else {
					putleb128(count);
					putbyte(byte);
					count = 0;
					last = byte;
				}
				if (j == 1)
					putleb128(count);
			} else {
				if (count >= 0) {
					putleb128(count);
					count = -1;
					last = -1;
				}
				putbyte(byte);
			}
		}
	} else {
		fprintf(stderr, "%s: run length decoding %d bytes\n", argv[0], bytes);
		for (int j = bytes; j; --j) {
			int byte = getbyte();
			putbyte(byte);
			if (byte == 0 || byte == 255)
				for (int i = getleb128(); i; --i, --j)
					putbyte(byte);
		}
	}
	double change = 100.0 * (wrote_bytes - read_bytes) / read_bytes;
	fprintf(stderr, "%s: read %d and wrote %d bytes %+.2f%%\n", argv[0], read_bytes, wrote_bytes, change);
	return 0;
}

