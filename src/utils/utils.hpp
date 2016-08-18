#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

unsigned int reverseBits(int N, unsigned int num);

double bound(double min, double max, double val);
unsigned int calculateHammingDistance(uint8_t a, uint8_t b);
#endif
