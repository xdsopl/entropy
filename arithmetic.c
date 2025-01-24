/*
Arithmetic coding of binary streams

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

int sma(int val, int len) {
	static int hist[1024];
	static int pos, sum;
	sum += val - hist[pos];
	hist[pos] = val;
	if (++pos >= len)
		pos = 0;
	return sum;
}

int clamp(int x, int a, int b) {
	return x < a ? a : x > b ? b : x;
}

const int code_bits = 16;
const int top_value = (1 << code_bits) - 1;
const int quarter = top_value / 4 + 1;
const int half = 2 * quarter;
const int third = 3 * quarter;
const int max_freq = 255;

int encode(int bit, int freq) {
	return 0;
}

int decode(int freq) {
	return 0;
}

int putac(int bit) {
	static int init, freq;
	if (!init) {
		init = 1;
		for (int i = 0; i <= max_freq; ++i)
			freq = clamp(sma(i & 1, max_freq), 1, max_freq - 1);
		//fprintf(stderr, "freq = %d\n", freq);
	}
	if (encode(bit, freq))
		return -1;
	freq = clamp(sma(bit, max_freq), 1, max_freq - 1);
	return 0;
}

int getac() {
	static int init, freq;
	if (!init) {
		init = 1;
		for (int i = 0; i <= max_freq; ++i)
			freq = clamp(sma(i & 1, max_freq), 1, max_freq - 1);
		//fprintf(stderr, "freq = %d\n", freq);
	}
	int bit = decode(freq);
	if (bit < 0)
		return -1;
	freq = clamp(sma(bit, max_freq), 1, max_freq - 1);
	return bit;
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
		for (int bits = 8 * bytes; bits; --bits)
			putac(getbit());
		flush_bits();
	} else {
		for (int bits = 8 * bytes; bits; --bits)
			putbit(getac());
	}
	double change = 100.0 * (wrote_bytes - read_bytes) / read_bytes;
	fprintf(stderr, "%s: %s %d to %d bytes %+.2f%%\n", argv[0], enc ? "encoded" : "decoded", read_bytes, wrote_bytes, change);
	return 0;
}

