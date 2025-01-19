/*
Copy stream of bits

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

int main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	if (*argv[1] != 'e' && *argv[1] != 'd')
		return 1;
	int bytes = getleb128();
	if (bytes <= 0)
		return 1;
	putleb128(bytes);
	while (bytes--)
		putbyte(getbyte());
	fprintf(stderr, "%s: read %d and wrote %d bytes\n", argv[0], read_bytes, wrote_bytes);
	return 0;
}

