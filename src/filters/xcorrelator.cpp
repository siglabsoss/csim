/*
 * XCorrelator.cpp
 *
 *  Created on: Jul 28, 2016
 *      Author: ubuntu
 */

#include <filters/xcorrelator.hpp>

using namespace std;

XCorrelator::XCorrelator(int N) :
		m_n(N)
{}

vector<FixedComplex32 > XCorrelator::xCorrelate(vector<FixedComplex32 > x, vector<FixedComplex32 > y)
{
    vector<FixedComplex32 > p1 = fft(x);//output is FFT(x)
    vector<FixedComplex32 > p2 = fft(y);//output is FFT(y)
    assert(p1.size() == m_n); //Same number of outputs into FFT as inputs
    assert(p2.size() == m_n); //Same number of outputs into FFT as inputs

    for (int i = 0; i < p2.size(); i++) {
        p2[i].imag(p2[i].imag() * -1);
    }//conj(FFT(Y)))
    for (int i = 0; i < p1.size(); i++) {
        p1[i] = p1[i] * p2[i];
    }//FFT(x) * conj(FFT(Y))

    p1 = ifft(p1);// IFFT(FFT(x) * conj(FFT(y)))

    return fftshift(p1);
}

vector<FixedComplex32 > XCorrelator::fft(vector<FixedComplex32 > vals)
{
    fixedfft fft(m_n);
    vector<FixedComplex32 > outputs;
    vector<FixedComplex32 > outputsBitReversed;

    filter_io_t data;
    	for (int i = 0; i < 2; i++) {
    		for (int j = 0; j < m_n; j++) {
    			data = vals[j];
    			fft.input(data);
    			bool test = fft.output(data);
    			if (test) {
    				outputsBitReversed.push_back(data.fcn32);
    			}//If output is ready
    		}//Insert all input
    	}//Insert input again to get output

    for (int i = 0; i < m_n; i++) {
             outputs.push_back(outputsBitReversed[reverseBits(m_n, i)]);
     }//Reformats data in correct order

    return outputs;
}

vector<FixedComplex32 > XCorrelator::ifft(vector<FixedComplex32> vals)
{
	fixedifft ifft(m_n);
	vector<FixedComplex32 > outputs;
	vector<FixedComplex32 > outputsBitReversed;

	filter_io_t data;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < m_n; j++) {
			data = vals[j];
			ifft.input(data);
			bool test = ifft.output(data);
			if (test) {
				outputsBitReversed.push_back(data.fcn32);
			}//If output is ready
		}//Insert all input
	}//Insert input again to get output

	for (int i = 0; i < m_n; i++) {
		outputs.push_back(outputsBitReversed[reverseBits(m_n, i)]);
	}//Reformats data in correct order

	return outputs;
}


vector<FixedComplex32 > XCorrelator::fftshift(vector<FixedComplex32 > vals)
{
    int size = vals.size() >> 1; //size/2
    if (vals.size() % 2 == 1) {
        size++; //Take ceiling
    }

    FixedComplex32 temp;//to hold swap

    for (int i = 0; i < size; i++) {
        temp = vals[i];
        vals[i] = vals[size+i];
        vals[size+i] = temp;
    }//Swaps front half and back half of vector
    vals.erase(vals.begin());//Erase first value to match octave output
    return vals;
}//Swaps front half and back half of vector.




