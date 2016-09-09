#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <fstream>
#include <types/fixedcomplex.hpp>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <string>
#include <stdint.h>

template <typename T>
std::vector<T> readComplexFromCSV(const std::string &inFile, double scaleDownFactor = 1.0);

unsigned int reverseBits(int N, unsigned int num);

double bound(double min, double max, double val);
unsigned int calculateHammingDistance(uint8_t a, uint8_t b);

namespace utils
{
std::unique_ptr<sc_fix> createDynamicFixedPoint(double val, size_t bitWidth, size_t &shiftBits);
unsigned getShiftAmount(double coeff);
unsigned getIntegerBits(double coeff);
bool addition32DoesOverflow(int32_t a, int32_t b);
};

#endif
