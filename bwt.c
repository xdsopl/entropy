/*
Burrows–Wheeler transform

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

#define BLOCK_POWER 10
#define BLOCK_SIZE (1 << BLOCK_POWER)

static int bwt_length;
static const unsigned char *bwt_input;

// only works for bwt_length % 8 == 0
static inline int bwt_byte(int p) {
	int i = p & 7, j = p >> 3;
	int l = bwt_input[j];
	int h = bwt_input[(j + 1) % (bwt_length >> 3)];
	return ((l >> i) | (h << (8 - i))) & 255;
}

static int bwt_compare(const void *a, const void *b) {
	int x = *(const int *)a;
	int y = *(const int *)b;
	int bits = bwt_length;
	while (bits >= 8) {
		bits -= 8;
		int L = bwt_byte(x);
		int R = bwt_byte(y);
		if (L != R) {
			int i = __builtin_ctz(L ^ R);
			int l = (L >> i) & 1;
			int r = (R >> i) & 1;
			return l - r;
		}
		x = (x + 8) % bwt_length;
		y = (y + 8) % bwt_length;
	}
	while (bits--) {
		int l = (bwt_input[x / 8] >> (x % 8)) & 1;
		int r = (bwt_input[y / 8] >> (y % 8)) & 1;
		if (l != r)
			return l - r;
		x = (x + 1) % bwt_length;
		y = (y + 1) % bwt_length;
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

static inline int bwt_get_bit(const unsigned char *array, int index) {
	return (array[index / 8] >> (index % 8)) & 1;
}

static inline void bwt_set_bit(unsigned char *array, int index, int value) {
	array[index / 8] = (array[index / 8] & ~(1 << (index % 8))) | (!!value << (index % 8));
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
		bwt_set_bit(output, i, bwt_get_bit(input, index - 1));
	}
	return row;
}

void ibwt(unsigned char *output, const unsigned char *input, int length, int row) {
	static int pref[BLOCK_SIZE];
	int count0 = 0, count1 = 0;
	for (int i = 0; i < length; ++i)
		pref[i] = bwt_get_bit(input, i) ? count1++ : count0++;
	int offset0 = 0, offset1 = count0;
	for (int i = length-1; i >= 0; --i) {
		int value = bwt_get_bit(input, row);
		bwt_set_bit(output, i, value);
		row = pref[row] + (value ? offset1 : offset0);
	}
}

int main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	if (*argv[1] != 'f' && *argv[1] != 'b')
		return 1;
	int fwd = *argv[1] == 'f';
	static unsigned char input[BLOCK_SIZE/8], output[BLOCK_SIZE/8];
	if (fwd) {
		int bytes = getleb128();
		if (bytes <= 0)
			return 1;
		int blocks = (8 * bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
		int extra = (blocks * BLOCK_POWER + 7) / 8 + 3;
		putleb128(bytes + extra);
		write_bits(extra, 24);
		for (int length = 0; bytes; --bytes) {
			input[length++] = getbyte();
			if (length >= BLOCK_SIZE/8 || bytes == 1) {
				int row = bwt(output, input, length*8);
				for (int i = 0; i < length; ++i)
					write_bits(output[i], 8);
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
		for (int length = 0; bytes; --bytes) {
			int byte;
			if (read_bits(&byte, 8))
				return 1;
			input[length++] = byte;
			if (length >= BLOCK_SIZE/8 || bytes == 1) {
				int row;
				if (read_bits(&row, BLOCK_POWER))
					return 1;
				ibwt(output, input, length*8, row);
				for (int i = 0; i < length; ++i)
					if (putbyte(output[i]))
						return 1;
				length = 0;
			}
		}
	}
	double change = 100.0 * (bytes_written - bytes_read) / bytes_read;
	fprintf(stderr, "%s: %s transformed %d to %d bytes %+.2f%%\n", argv[0], fwd ? "forward" : "backward", bytes_read, bytes_written, change);
	return 0;
}

