#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <fstream>
#include <types/fixedcomplex.hpp>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <string>
#include <stdint.h>

unsigned int reverseBits(int N, unsigned int num);
std::vector<FixedComplex16> complexRead16Scaled(std::string inFile);
std::vector<FixedComplex16> complexRead16Unscaled(std::string inFile);
std::vector<FixedComplex32> complexRead32Scaled(std::string inFile);
std::vector<FixedComplex32> complexRead32Unscaled(std::string inFile);
std::vector<FixedComplex64> complexRead64Scaled(std::string inFile);
std::vector<FixedComplex64> complexRead64Unscaled(std::string inFile);
void print(FixedComplex32 x);
void print(std::vector<FixedComplex32> x, int begin = 0, int end = 0);
double bound(double min, double max, double val);
unsigned int calculateHammingDistance(uint8_t a, uint8_t b);
#endif
