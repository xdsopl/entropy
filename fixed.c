/*
Produce a stream of bits with a fixed Bernoulli distribution

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

int main(int argc, char **argv) {
	if (argc != 4)
		return 1;
	if (*argv[1] != 'g' && *argv[1] != 'v')
		return 1;
	int gen = *argv[1] == 'g';
	int bytes = atoi(argv[2]);
	if (bytes <= 0)
		return 1;
	double prob = atof(argv[3]);
	if (prob < 0 || prob > 1)
		return 1;
	unsigned value = 4294967295 * prob;
	if (gen) {
		putleb128(bytes);
		while (bytes--) {
			int byte = 0;
			for (int i = 0; i < 8; ++i) {
				int bit = xorshift32() <= value;
				byte |= bit << i;
			}
			putbyte(byte);
		}
	} else {
		if (getleb128() != bytes)
			return 1;
		while (bytes--) {
			int byte = 0;
			for (int i = 0; i < 8; ++i) {
				int bit = xorshift32() <= value;
				byte |= bit << i;
			}
			if (getbyte() != byte)
				return 1;
		}
	}
	fprintf(stderr, "%s: %s %d bytes with probability %f\n", argv[0], gen ? "generated" : "verified", gen ? bytes_written : bytes_read, prob);
	return 0;
}

