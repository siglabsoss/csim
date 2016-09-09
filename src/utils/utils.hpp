#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <fstream>
#include <types/fixedcomplex.hpp>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <string>
#include <stdint.h>

template <typename T>
std::vector<T> complexReadUnscaled(const std::string &inFile);

unsigned int reverseBits(int N, unsigned int num);
std::vector<FixedComplex16> complexRead16Scaled(std::string inFile);//Reads file and divides values by 32768. Returns vector of FixedComplex16
std::vector<FixedComplex16> complexRead16Unscaled(std::string inFile); //Reads file and returns vector of FixedComplex16
std::vector<FixedComplex32> complexRead32Scaled(std::string inFile); //Reads file and divides values by 32768. Returns vector of FixedComplex32
std::vector<FixedComplex32> complexRead32Unscaled(std::string inFile); //Reads file and returns vector of FixedComplex32
std::vector<FixedComplex64> complexRead64Scaled(std::string inFile); //Reads file and divides values by 32768. Returns vector of FixedComplex64
std::vector<FixedComplex64> complexRead64Unscaled(std::string inFile); //Reads file and returns vector of FixedComplex64
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
