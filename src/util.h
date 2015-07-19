#pragma once
#include <stdint.h>
void slurp(const char *filename, uint8_t **out, long *filesize);
uint8_t select_bits(uint8_t in, char start, char end);
