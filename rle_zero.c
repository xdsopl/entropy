/*
Variable length integer based run length encoding of mostly zero bits

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

int putval(int val) {
	static int order;
	while (val >= 1 << order) {
		if (putbit(0))
			return -1;
		val -= 1 << order;
		order += 1;
	}
	if (putbit(1))
		return -1;
	if (write_bits(val, order))
		return -1;
	order -= 1;
	if (order < 0)
		order = 0;
	return 0;
}

int getval() {
	static int order;
	int val, sum = 0, ret;
	while ((ret = getbit()) == 0) {
		sum += 1 << order;
		order += 1;
	}
	if (ret < 0)
		return -1;
	if (read_bits(&val, order))
		return -1;
	order -= 1;
	if (order < 0)
		order = 0;
	return val + sum;
}

int putrle(int bit) {
	static int cnt;
	if (cnt < 0)
		return -1;
	if (bit)
		return cnt = putval(cnt);
	cnt++;
	return 0;
}

int getrle() {
	static int cnt;
	if (cnt < 0)
		return -1;
	if (!cnt) {
		cnt = getval();
		if (cnt < 0)
			return -1;
		return !cnt;
	}
	return cnt-- == 1;
}

int main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	if (*argv[1] != 'e' && *argv[1] != 'd')
		return 1;
	int bytes = getleb128();
	if (bytes <= 0)
		return 1;
	putleb128(bytes);
	if (*argv[1] == 'e') {
		fprintf(stderr, "%s: run length encoding %d bytes\n", argv[0], bytes);
		for (int bits = 8 * bytes; bits; --bits)
			putrle(getbit());
		putrle(1); // flush always
		flush_bits();
	} else {
		fprintf(stderr, "%s: run length decoding %d bytes\n", argv[0], bytes);
		for (int bits = 8 * bytes; bits; --bits)
			putbit(getrle());
	}
	double change = 100.0 * (wrote_bytes - read_bytes) / read_bytes;
	fprintf(stderr, "%s: read %d and wrote %d bytes %+.2f%%\n", argv[0], read_bytes, wrote_bytes, change);
	return 0;
}

