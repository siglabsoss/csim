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
    vector<FixedComplex32 > xCorrelate(vector<FixedComplex32 > x,vector<FixedComplex32 > y); //cross correlates 2 vectors of FixedComplex<32>
    vector<FixedComplex32 > fft(vector<FixedComplex32 > vals);//computes fft
    vector<FixedComplex32 > ifft(vector<FixedComplex32 > vals);//computes ifft
    vector<FixedComplex32 > fftshift(vector<FixedComplex32 > vals);//fftshifts the results

    int m_n;// n point fft and ifft

};

