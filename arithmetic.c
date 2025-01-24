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

const int code_bits = 16;
const int top_value = (1 << code_bits) - 1;
const int quarter = top_value / 4 + 1;
const int half = 2 * quarter;
const int third = 3 * quarter;
const int factor = 256;

int encode(int symbol, int freq) {
	static int follow, low, high = top_value;
	if (symbol < 0) {
		++follow;
		int out = low < quarter;
		if (putbit(!out))
			return -1;
		while (follow) {
			if (putbit(out))
				return -1;
			--follow;
		}
		return 0;
	}
	int range = high - low + 1;
	int offset = (range * freq) / factor;
	if (symbol)
		low += offset;
	else
		high = low + offset - 1;
	while (1) {
		if (high < half) {
			if (putbit(0))
				return -1;
			while (follow) {
				if (putbit(1))
					return -1;
				--follow;
			}
		} else if (low >= half) {
			if (putbit(1))
				return -1;
			while (follow) {
				if (putbit(0))
					return -1;
				--follow;
			}
			low -= half;
			high -= half;
		} else if (low >= quarter && high < third) {
			++follow;
			low -= quarter;
			high -= quarter;
		} else {
			break;
		}
		low *= 2;
		high = 2 * high + 1;
	}
	return 0;
}

int decode(int freq) {
	static int value, low, high = top_value;
	if (freq < 0) {
		for (int i = 0; i < code_bits; ++i) {
			int bit = getbit();
			if (bit < 0)
				return -1;
			value <<= 1;
			value |= bit;
		}
		return 0;
	}
	int range = high - low + 1;
	int current = ((value - low + 1) * factor - 1) / range;
	int symbol = current >= freq;
	int offset = (range * freq) / factor;
	if (symbol)
		low += offset;
	else
		high = low + offset - 1;
	while (1) {
		if (high < half) {
		} else if (low >= half) {
			value -= half;
			low -= half;
			high -= half;
		} else if (low >= quarter && high < third) {
			value -= quarter;
			low -= quarter;
			high -= quarter;
		} else {
			break;
		}
		low *= 2;
		high = 2 * high + 1;
		value <<= 1;
		static int eof;
		if (!eof) {
			int bit = getbit();
			if (bit < 0) {
				eof = 1;
				bit = 0;
			}
			value |= bit;
		}
	}
	return symbol;
}

int putac(int bit) {
	static int init, freq;
	if (!init) {
		init = 1;
		for (int i = 0; i < factor; ++i)
			freq = 1 + sma(i & 1, factor - 2);
		//fprintf(stderr, "freq = %d\n", freq);
	}
	if (encode(bit, freq))
		return -1;
	freq = 1 + sma(!bit, factor - 2);
	//fprintf(stderr, "freq = %d\n", freq);
	return 0;
}

int getac() {
	static int init, freq;
	if (!init) {
		init = 1;
		for (int i = 0; i < factor; ++i)
			freq = 1 + sma(i & 1, factor - 2);
		if (decode(-1))
			return -1;
	}
	int bit = decode(freq);
	if (bit < 0)
		return -1;
	freq = 1 + sma(!bit, factor - 2);
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
		putac(-1);
		flush_bits();
	} else {
		for (int bits = 8 * bytes; bits; --bits)
			putbit(getac());
	}
	double change = 100.0 * (wrote_bytes - read_bytes) / read_bytes;
	fprintf(stderr, "%s: %s %d to %d bytes %+.2f%%\n", argv[0], enc ? "encoded" : "decoded", read_bytes, wrote_bytes, change);
	return 0;
}

