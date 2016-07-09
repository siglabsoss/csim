#include "butterfly_two_pt.h"

void butterfly_two_pt::butterfly_fft()
{
	result[0] = x + y;
	result[1] = x - y;
}
