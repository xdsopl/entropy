/*
Print simple moving average of popcnt

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

int popcnt(int x) {
	int cnt = 0;
	while (x) {
		++cnt;
		x &= x-1;
	}
	return cnt;
}

int main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	int len = atoi(argv[1]);
	if (len <= 0 || len >= 1024)
		return 1;
	double factor = 1.0 / (8 * len);
	for (int bytes = getleb128(); bytes; --bytes)
		printf("%f\n", factor * sma(popcnt(getbyte()), len));
	fprintf(stderr, "%s: read %d bytes\n", argv[0], read_bytes);
	return 0;
}

