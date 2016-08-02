/*
 * XCorrelator.cpp
 *
 *  Created on: Jul 28, 2016
 *      Author: ubuntu
 */

#include <xcorrelator.hpp>


using namespace std;

XCorrelator::XCorrelator(int N)
{
    m_n = N;
    m_fft = new fixedfft(N);
    m_ifft = new fixedifft(N);
}

vector<FixedComplex<32> > XCorrelator::xCorrelate(vector<FixedComplex<32> > x, vector<FixedComplex<32> > y)
{
    vector<FixedComplex<32> > p1 = fft(x);//FFT(x)
    vector<FixedComplex<32> > p2 = fft(y);//FFT(y)
    assert(p1.size() == m_n); //Same number of outputs into FFT as inputs
    assert(p2.size() == m_n); //Same number of outputs into FFT as inputs

    for (int i = 0; i < p2.size(); i++) {
        p2[i].imag = -p2[i].imag;
    }//conj(FFT(Y)))

    for (int i = 0; i < p1.size(); i++) {
        p1[i] = p1[i] * p2[i];
    }//FFT(x) * conj(FFT(Y))

    p1 = ifft(p1);// IFFT(FFT(x) * conj(FFT(Y)))

    return fftshift(p1);
}

vector<FixedComplex<32> > XCorrelator::fft(vector<FixedComplex<32> > vals)
{
    vector<FixedComplex<32> > bitReversedAnswers;
    vector<FixedComplex<32> > answers;
    filter_io_t data;
    data.type =  IO_TYPE_FIXED_COMPLEX_32;
    data.fc32 = FixedComplex<32>(0,0);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < m_n; j++) {
            data.fc32 = vals[j];
            m_fft->input(data);
            bool test = m_fft->output(data);
            if (test) {
                bitReversedAnswers.push_back(data.fc32);
            }//If an output is ready
        }//For every input
    }//run twice to get all outputs

    for (int i = 0; i < m_n; i++) {
             answers.push_back(bitReversedAnswers[reverseBits(m_n, i)]);
     }//Reformats data in correct order

    return answers;
}

vector<FixedComplex<32> > XCorrelator::ifft(vector<FixedComplex<32> > vals)
{
    vector<FixedComplex<32> > answers;
    vector<FixedComplex<32> > bitReversedAnswers;
    filter_io_t data;
    data.type =  IO_TYPE_FIXED_COMPLEX_32;
    data.fc32 = FixedComplex<32>(0,0);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < m_n; j++) {
            data.fc32 = vals[j];
            m_ifft->input(data);
            bool test = m_ifft->output(data);
            if (test) {
                bitReversedAnswers.push_back(data.fc32);
            }//If an output is ready
        }//For every input
    }//run twice to get all output

    FixedComplex<32> temp; //to hold swap

    for (int i = 0; i < m_n; i++) {
        temp.real = bitReversedAnswers[reverseBits(m_n, i)].real/m_n;
        temp.imag = bitReversedAnswers[reverseBits(m_n, i)].imag/m_n;
        answers.push_back(temp);
    }//Reformats data in correct order

    return answers;
}


vector<FixedComplex<32> > XCorrelator::fftshift(vector<FixedComplex<32> > vals)
{
    int size = vals.size() >> 1;
    if (vals.size() % 2 == 1) {
        size++; //Take ceiling
    }

    FixedComplex<32> temp;//to hold swap

    for (int i = 0; i < size; i++) {
        temp = vals[i];
        vals[i] = vals[size+i];
        vals[size+i] = temp;
    }
    vals.erase(vals.begin());//Erase first value to match octave output
    return vals;
}


XCorrelator::~XCorrelator()
{
    // TODO Auto-generated destructor stub
}



