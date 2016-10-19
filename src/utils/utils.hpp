#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <fstream>
#include <types/fixedcomplex.hpp>
#include <types/complexdouble.hpp>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <string>
#include <stdint.h>

namespace utils
{
template <typename T>
std::vector<T> readComplexFromCSV(const std::string &inFile);

unsigned int reverseBits(int N, unsigned int num);

double bound(double min, double max, double val);
unsigned int calculateHammingDistance(uint8_t a, uint8_t b);


int getShiftAmount(double coeff);
unsigned getIntegerBits(double coeff);
bool addition32DoesOverflow(int32_t a, int32_t b);
size_t calculateInt32ScaleExponent(const std::vector<ComplexDouble> &values);
};

#endif


#define _UNUSED_     __attribute__ ((unused))
