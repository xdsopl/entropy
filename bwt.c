/*
Burrows–Wheeler transform

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

#define ALPHABET_SIZE 2
#define BLOCK_POWER 8
#define BLOCK_SIZE (1 << BLOCK_POWER)

static int bwt_length;
static const unsigned char *bwt_input;

int bwt_compare(const void *a, const void *b) {
	int x = *(const int *)a;
	int y = *(const int *)b;
	for (int i = 0; i < bwt_length; ++i) {
		int l = bwt_input[(x + i) % bwt_length];
		int r = bwt_input[(y + i) % bwt_length];
		if (l != r)
			return l - r;
	}
	return 0;
}

void bwt_rot(int *output, const unsigned char *input, int length) {
	bwt_input = input;
	bwt_length = length;
	for (int i = 0; i < length; ++i)
		output[i] = i;
	qsort(output, length, sizeof(int), bwt_compare);
}

int bwt(unsigned char *output, const unsigned char *input, int length) {
	static int row, rot[BLOCK_SIZE];
	bwt_rot(rot, input, length);
	for (int i = 0; i < length; ++i) {
		int index = rot[i];
		if (index == 0) {
			index = length;
			row = i;
		}
		output[i] = input[index - 1];
	}
	return row;
}

void ibwt(unsigned char *output, const unsigned char *input, int length, int row) {
	static int count[ALPHABET_SIZE];
	for (int i = 0; i < ALPHABET_SIZE; ++i)
		count[i] = 0;
	static int pref[BLOCK_SIZE];
	for (int i = 0; i < length; ++i)
		pref[i] = count[input[i]]++;
	for (int i = 0, sum = 0; i < ALPHABET_SIZE; ++i) {
		int tmp = count[i] + sum;
		count[i] = sum;
		sum = tmp;
	}
	for (int i = length-1; i >= 0; --i) {
		output[i] = input[row];
		row = pref[row] + count[input[row]];
	}
}

int main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	if (*argv[1] != 'f' && *argv[1] != 'b')
		return 1;
	int fwd = *argv[1] == 'f';
	static unsigned char input[BLOCK_SIZE], output[BLOCK_SIZE];
	if (fwd) {
		int bytes = getleb128();
		if (bytes <= 0)
			return 1;
		int blocks = (8 * bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
		int extra = (blocks * BLOCK_POWER + 7) / 8 + 3;
		putleb128(bytes + extra);
		write_bits(extra, 24);
		for (int bits = 8 * bytes, length = 0; bits; --bits) {
			input[length++] = getbit();
			if (length >= BLOCK_SIZE || bits == 1) {
				int row = bwt(output, input, length);
				for (int i = 0; i < length; ++i)
					putbit(output[i]);
				write_bits(row, BLOCK_POWER);
				length = 0;
			}
		}
		flush_bits();
	} else {
		int bytes = getleb128();
		if (bytes <= 0)
			return 1;
		int extra;
		if (read_bits(&extra, 24))
			return 1;
		bytes -= extra;
		putleb128(bytes);
		for (int bits = 8 * bytes, length = 0; bits; --bits) {
			input[length++] = getbit();
			if (length >= BLOCK_SIZE || bits == 1) {
				int row;
				if (read_bits(&row, BLOCK_POWER))
					return 1;
				ibwt(output, input, length, row);
				for (int i = 0; i < length; ++i)
					putbit(output[i]);
				length = 0;
			}
		}
	}
	double change = 100.0 * (bytes_written - bytes_read) / bytes_read;
	fprintf(stderr, "%s: %s transformed %d to %d bytes %+.2f%%\n", argv[0], fwd ? "forward" : "backward", bytes_read, bytes_written, change);
	return 0;
}

