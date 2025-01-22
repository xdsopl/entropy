/*
Byte based run length encoding of a stream of bits

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

int putrle(int byte) {
	static int prev = -1, count = -1;
	if (byte < 0) {
		if (count < 0)
			return 0;
		else
			return putbyte(count);
	}
	if (byte == 0 || byte == 255) {
		if (prev == byte) {
			if (count < 255) {
				++count;
			} else {
				if (putbyte(count))
					return -1;
				if (putbyte(byte))
					return -1;
				count = 0;
			}
		} else if (count < 0) {
			if (putbyte(byte))
				return -1;
			count = 0;
			prev = byte;
		} else {
			if (putbyte(count))
				return -1;
			if (putbyte(byte))
				return -1;
			count = 0;
			prev = byte;
		}
	} else {
		if (count >= 0) {
			if (putbyte(count))
				return -1;
			count = -1;
			prev = -1;
		}
		if (putbyte(byte))
			return -1;
	}
	return 0;
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
	if (enc) {
		while (bytes--)
			putrle(getbyte());
		putrle(-1); // flush
	} else {
		while (bytes--) {
			int byte = getbyte();
			putbyte(byte);
			if (byte == 0 || byte == 255)
				for (int i = getbyte(); i; --i, --bytes)
					putbyte(byte);
		}
	}
	double change = 100.0 * (wrote_bytes - read_bytes) / read_bytes;
	fprintf(stderr, "%s: %s %d to %d bytes %+.2f%%\n", argv[0], enc ? "encoded" : "decoded", read_bytes, wrote_bytes, change);
	return 0;
}

