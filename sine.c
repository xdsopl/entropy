/*
Produce a stream of bits with a sinusoidal Bernoulli distribution

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

int main(int argc, char **argv) {
	if (argc != 4)
		return 1;
	if (*argv[1] != 'g' && *argv[1] != 'v')
		return 1;
	int bytes = atoi(argv[2]);
	if (bytes <= 0)
		return 1;
	int periods = atoi(argv[3]);
	if (periods <= 0)
		return 1;
	if (*argv[1] == 'g') {
		fprintf(stderr, "%s: generating %d bytes with %d periods\n", argv[0], bytes, periods);
		putleb128(bytes);
		for (int j = 0; j < bytes; ++j) {
			unsigned value = 4294967295 * 0.5 * (1.0 - cos(6.2832 * periods * j / bytes));
			int byte = 0;
			for (int i = 0; i < 8; ++i) {
				int bit = xorshift32() <= value;
				byte |= bit << i;
			}
			putbyte(byte);
		}
		fprintf(stderr, "%s: wrote %d bytes\n", argv[0], wrote_bytes);
	} else {
		fprintf(stderr, "%s: verifying %d bytes with %d periods\n", argv[0], bytes, periods);
		if (getleb128() != bytes)
			return 1;
		for (int j = 0; j < bytes; ++j) {
			unsigned value = 4294967295 * 0.5 * (1.0 - cos(6.2832 * periods * j / bytes));
			int byte = 0;
			for (int i = 0; i < 8; ++i) {
				int bit = xorshift32() <= value;
				byte |= bit << i;
			}
			if (getbyte() != byte)
				return 1;
		}
		fprintf(stderr, "%s: read %d bytes\n", argv[0], read_bytes);
	}
	return 0;
}

