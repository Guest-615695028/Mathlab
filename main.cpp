#include <stdio.h>
#include <stdint.h>
#include "basics.hpp"
using namespace Mathlab;
int main() {
	FILE *f = fopen("1.bmp", "rb");
	uint8_t i = 0, a = 0;
	constexpr int ROWS = 64;
	for (i = 0; i < ROWS * 2; i += printf("+---")) {}
	i = putchar('\n');
	while (fread(&i, 1, 1, f) > 0) {
		printf("%02x", i);
		if (++a % ROWS == 0) putchar('\n');
	}
	return fclose(f);
}