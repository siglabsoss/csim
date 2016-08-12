#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <fstream>
#include <types/fixedcomplex.hpp>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <string>
unsigned int reverseBits(int N, unsigned int num);
vector<FixedComplex16> complexRead16Scaled(string inFile);
vector<FixedComplex16> complexRead16Unscaled(string inFile);
double bound(double min, double max, double val);
#endif
