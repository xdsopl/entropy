/*
Variable length integer based coding of byte-blocked stream of bits

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

int putval(int val) {
	int cnt = 0, top = 1;
	while (top <= val) {
		cnt += 1;
		top = 1 << cnt;
		if (putbit(1))
			return -1;
	}
	if (putbit(0))
		return -1;
	if (cnt > 0) {
		cnt -= 1;
		val -= top / 2;
		if (write_bits(val, cnt))
			return -1;
	}
	return 0;
}

int getval() {
	int val = 0, cnt = 0, top = 1, ret;
	while ((ret = getbit()) == 1) {
		cnt += 1;
		top = 1 << cnt;
	}
	if (ret < 0)
		return -1;
	if (cnt > 0) {
		cnt -= 1;
		if (read_bits(&val, cnt))
			return -1;
		val += top / 2;
	}
	return val;
}

static int *hist;
int comp(const void *a, const void *b) {
	int x = hist[*(int *)a];
	int y = hist[*(int *)b];
	return (x < y) - (x > y);
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
		hist = calloc(256, sizeof(int));
		int *input = calloc(bytes, sizeof(int));
		for (int *p = input; p < input + bytes; ++p)
			++hist[*p = getbyte()];
		int *perm = calloc(256, sizeof(int));
		for (int i = 0; i < 256; ++i)
			perm[i] = i;
		qsort(perm, 256, sizeof(int), comp);
		int count = 0;
		while (count < 256 && hist[perm[count]])
			++count;
		putbyte(count - 1);
		for (int i = 0; i < count; ++i)
			putbyte(perm[i]);
		int *imap = calloc(256, sizeof(int));
		for (int i = 0; i < count; ++i)
			imap[perm[i]] = i;
		for (int *p = input; p < input + bytes; ++p)
			putval(imap[*p]);
		flush_bits();
		free(input);
		free(hist);
		free(imap);
		free(perm);
	} else {
		int count = getbyte() + 1;
		int *perm = calloc(count, sizeof(int));
		for (int i = 0; i < count; ++i)
			perm[i] = getbyte();
		while (bytes--)
			putbyte(perm[getval()]);
		free(perm);
	}
	double change = 100.0 * (bytes_written - bytes_read) / bytes_read;
	fprintf(stderr, "%s: %s %d to %d bytes %+.2f%%\n", argv[0], enc ? "encoded" : "decoded", bytes_read, bytes_written, change);
	return 0;
}

