/*
Xor transform

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

int main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	if (*argv[1] != 'f' && *argv[1] != 'b')
		return 1;
	int fwd = *argv[1] == 'f';
	int bytes = getleb128();
	if (bytes <= 0)
		return 1;
	putleb128(bytes);
	if (fwd) {
		for (int bits = 8 * bytes, prev = 0; bits; --bits) {
			int bit = getbit();
			putbit(bit ^ prev);
			prev = bit;
		}
	} else {
		for (int bits = 8 * bytes, prev = 0; bits; --bits)
			putbit(prev ^= getbit());
	}
	double change = 100.0 * (bytes_written - bytes_read) / bytes_read;
	fprintf(stderr, "%s: %s transformed %d to %d bytes %+.2f%%\n", argv[0], fwd ? "forward" : "backward", bytes_read, bytes_written, change);
	return 0;
}

