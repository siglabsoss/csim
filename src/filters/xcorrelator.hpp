/*
 * XCorrelator.hpp
 *
 *  Created on: Jul 28, 2016
 *      Author: ubuntu
 */

#pragma once
#include <filters/fixedfft.hpp>
#include <filters/fixedifft.hpp>
#include <vector>
#include <utils/utils.hpp>
#include <queue>

class XCorrelator
{
public:
    XCorrelator(int N);
    std::vector<FixedComplex32 > xCorrelate(std::vector<FixedComplex32 > x,std::vector<FixedComplex32 > y); //cross correlates 2 vectors of FixedComplex<32>
    std::vector<FixedComplex32 > fft(std::vector<FixedComplex32 > vals);//computes fft
    std::vector<FixedComplex32 > ifft(std::vector<FixedComplex32 > vals);//computes ifft
    std::vector<FixedComplex32 > fftshift(std::vector<FixedComplex32 > vals);//fftshifts the results

    int m_n;// n point fft and ifft

};

