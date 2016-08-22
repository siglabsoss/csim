#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <fstream>
#include <types/fixedcomplex.hpp>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <string>
#include <stdint.h>

unsigned int reverseBits(int N, unsigned int num);
vector<FixedComplex16> complexRead16Scaled(string inFile);
vector<FixedComplex16> complexRead16Unscaled(string inFile);
vector<FixedComplex32> complexRead32Scaled(string inFile);
vector<FixedComplex32> complexRead32Unscaled(string inFile);
vector<FixedComplex64> complexRead64Scaled(string inFile);
vector<FixedComplex64> complexRead64Unscaled(string inFile);
void print(FixedComplex32 x);
void print(vector<FixedComplex32> x, int begin = 0, int end = 0);
double bound(double min, double max, double val);
unsigned int calculateHammingDistance(uint8_t a, uint8_t b);
#endif
