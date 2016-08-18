#include <cmath>
#include <utils/utils.hpp>

unsigned int reverseBits(int N, unsigned int num)
{
//    unsigned int  NO_OF_BITS = sizeof(num) * 8;
    unsigned int NO_OF_BITS = log2(N);
    unsigned int reverse_num = 0;
    int i;
    for (i = 0; i < NO_OF_BITS; i++) {
        if ((num & (1 << i)))
            reverse_num |= 1 << ((NO_OF_BITS - 1) - i);
    }
    return reverse_num;
}

double bound(double min, double max, double val)
{
    if (val > max) {
        val = max;
    } else if (val < min) {
        val = min;
    }
    return val;
}

unsigned int calculateHammingDistance(uint8_t a, uint8_t b)
{
    unsigned int count = 0;
    uint8_t diff = a ^ b;
    for (unsigned i = 0; i < 8; i++) {
        if (diff & (1 << i)) {
            count++;
        }
    }
    return count;
}
