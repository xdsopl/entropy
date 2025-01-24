/*
Arithmetic coding of binary streams

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

const int code_bits = 16;
const int top_value = (1 << code_bits) - 1;
const int quarter = top_value / 4 + 1;
const int half = 2 * quarter;
const int third = 3 * quarter;
const int factor = 32;

int putbits(int bit, int follow) {
	if (bit < 0 || follow < 0)
		return -1;
	if (putbit(bit))
		return -1;
	while (follow--)
		if (putbit(!bit))
			return -1;
	return 0;
}

int encode(int bit, int freq) {
	static int follow, low, high = top_value;
	if (bit < 0)
		return follow = putbits(low >= quarter, follow + 1);
	int range = high - low + 1;
	int offset = (range * freq) / factor;
	if (bit)
		low += offset;
	else
		high = low + offset - 1;
	while (1) {
		if (high < half) {
			if ((follow = putbits(0, follow)))
				return -1;
		} else if (low >= half) {
			if ((follow = putbits(1, follow)))
				return -1;
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

int getabit() {
	static int eof_cnt;
	if (eof_cnt > code_bits - 2)
		return -1;
	if (eof_cnt) {
		++eof_cnt;
		return 0;
	}
	int bit = getbit();
	if (bit < 0) {
		eof_cnt = 1;
		return 0;
	}
	return bit;
}

int decode(int freq) {
	static int value, low, high = top_value;
	if (freq < 0) {
		for (int i = 0; i < code_bits; ++i) {
			value <<= 1;
			value |= getabit();
		}
		return 0;
	}
	int range = high - low + 1;
	int bit = (value - low + 1) * factor >= freq * range + 1;
	int offset = (range * freq) / factor;
	if (bit)
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
		value |= getabit();
	}
	return bit;
}

int freq32(int bit) {
	static int hist = 1431655765, sum = 16;
	int old = (hist >> 31) & 1;
	sum += !bit - old;
	hist <<= 1;
	hist |= !bit;
	return sum < 1 ? 1 : sum > 31 ? 31 : sum;
}

int putac(int bit) {
	static int freq = 16;
	if (encode(bit, freq))
		return -1;
	freq = freq32(bit);
	return 0;
}

int getac() {
	static int init, freq = 16;
	if (!init) {
		init = 1;
		if (decode(-1))
			return -1;
	}
	int bit = decode(freq);
	if (bit < 0)
		return -1;
	freq = freq32(bit);
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
		putac(-1); // flush
		flush_bits();
	} else {
		for (int bits = 8 * bytes; bits; --bits)
			putbit(getac());
	}
	double change = 100.0 * (wrote_bytes - read_bytes) / read_bytes;
	fprintf(stderr, "%s: %s %d to %d bytes %+.2f%%\n", argv[0], enc ? "encoded" : "decoded", read_bytes, wrote_bytes, change);
	return 0;
}

