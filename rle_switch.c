/*
Variable length integer based run length encoding that switches between zeros and ones

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

int putval(int val, int ctx) {
	static int order[2];
	int cnt = order[ctx];
	while (val >= 1 << cnt) {
		if (putbit(0))
			return -1;
		val -= 1 << cnt;
		cnt += 1;
	}
	if (putbit(1))
		return -1;
	if (write_bits(val, cnt))
		return -1;
	cnt -= 1;
	if (cnt < 0)
		cnt = 0;
	order[ctx] = (order[ctx] + cnt) / 2;
	return 0;
}

int getval(int ctx) {
	static int order[2];
	int cnt = order[ctx], sum = 0, ret, val;
	while ((ret = getbit()) == 0) {
		sum += 1 << cnt;
		cnt += 1;
	}
	if (ret < 0)
		return -1;
	if (read_bits(&val, cnt))
		return -1;
	cnt -= 1;
	if (cnt < 0)
		cnt = 0;
	order[ctx] = (order[ctx] + cnt) / 2;
	return val + sum;
}

int putrle(int bit) {
	static int prv = 2, cnt;
	if (cnt < 0 || prv < 0)
		return -1;
	if (bit < 0)
		return cnt = putval(cnt, prv);
	if (prv > 1)
		return putbit(prv = bit);
	if (bit != prv) {
		cnt = putval(cnt, prv);
		prv = bit;
		return cnt;
	}
	cnt++;
	return 0;
}

int getrle() {
	static int bit = 2, cnt;
	if (cnt < 0 || bit < 0)
		return -1;
	if (bit > 1) {
		bit = getbit();
		if (bit < 0)
			return -1;
		bit = !bit;
	}
	if (!cnt) {
		bit = !bit;
		cnt = getval(bit);
		if (cnt < 0)
			return -1;
	} else {
		--cnt;
	}
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
			putrle(getbit());
		putrle(-1); // flush
		flush_bits();
	} else {
		for (int bits = 8 * bytes; bits; --bits)
			putbit(getrle());
	}
	double change = 100.0 * (bytes_written - bytes_read) / bytes_read;
	fprintf(stderr, "%s: %s %d to %d bytes %+.2f%%\n", argv[0], enc ? "encoded" : "decoded", bytes_read, bytes_written, change);
	return 0;
}

