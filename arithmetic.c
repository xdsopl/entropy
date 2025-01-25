/*
Arithmetic coding of binary streams

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

const int factor = 32;
const int code_bits = 16;
const int max_value = (1 << code_bits) - 1;
const int first_half = 1 << (code_bits - 1);
const int first_quarter = 1 << (code_bits - 2);
const int last_quarter = first_quarter | first_half;

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
	static int follow, lower, upper = max_value;
	if (bit < 0)
		return follow = putbits(lower >= first_quarter, follow + 1);
	int range = upper - lower + 1;
	int point = range * freq;
	int offset = point / factor;
	if (bit)
		lower += offset;
	else
		upper = lower + offset - 1;
	while (1) {
		if (upper < first_half) {
			if ((follow = putbits(0, follow)))
				return -1;
		} else if (lower >= first_half) {
			if ((follow = putbits(1, follow)))
				return -1;
			lower -= first_half;
			upper -= first_half;
		} else if (lower >= first_quarter && upper < last_quarter) {
			++follow;
			lower -= first_quarter;
			upper -= first_quarter;
		} else {
			break;
		}
		lower <<= 1;
		upper <<= 1;
		upper |= 1;
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
	static int value, lower, upper = max_value;
	if (freq < 0) {
		for (int i = 0; i < code_bits; ++i) {
			value <<= 1;
			value |= getabit();
		}
		return 0;
	}
	int range = upper - lower + 1;
	int point = range * freq;
	int bit = (value - lower + 1) * factor >= point + 1;
	int offset = point / factor;
	if (bit)
		lower += offset;
	else
		upper = lower + offset - 1;
	while (1) {
		if (upper < first_half) {
			// nothing to see here
		} else if (lower >= first_half) {
			value -= first_half;
			lower -= first_half;
			upper -= first_half;
		} else if (lower >= first_quarter && upper < last_quarter) {
			value -= first_quarter;
			lower -= first_quarter;
			upper -= first_quarter;
		} else {
			break;
		}
		lower <<= 1;
		upper <<= 1;
		upper |= 1;
		value <<= 1;
		int ret = getabit();
		if (ret < 0)
			return -1;
		value |= ret;
	}
	return bit;
}

int freq32(int bit) {
	static int past = 0x55555555, freq = 16;
	if (!bit)
		++freq;
	if (past & (1 << 31))
		--freq;
	past <<= 1;
	past |= !bit;
	return freq < 1 ? 1 : freq > 31 ? 31 : freq;
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

