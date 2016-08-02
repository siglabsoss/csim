/*
 * XCorrelator.cpp
 *
 *  Created on: Jul 28, 2016
 *      Author: ubuntu
 */
#include <fstream>
#include <xcorrelator.hpp>
string outfile10("../csim/data/xcorrelator/output/data.txt");
ofstream out10(outfile10.c_str());

namespace std {

XCorrelator::XCorrelator(int N)
{
    m_n = N;
    m_fft = new fixedfft(N);
    m_ifft = new fixedifft(N);
}

vector<FixedComplex<32> > XCorrelator::xCorrelate(vector<FixedComplex<32> > x, vector<FixedComplex<32> > y)
{
    if (!out10.is_open())
    {
        cout << "WHY";
    }
    vector<FixedComplex<32> > p1 = fft(x);
    vector<FixedComplex<32> > p2 = fft(y);
    assert(p1.size() == m_n);
    assert(p2.size() == m_n);
    vector<FixedComplex<32> > temp;


    for (int i = 0; i < p1.size(); i++) {
            temp.push_back(FixedComplex<32>(0,0));
    }//Reformats data in correct order

    for (int i = 0; i < p1.size(); i++) {
            temp[i] = (p1[reverseBits(m_n, i)]);
    }//Reformats data in correct order

    for (int i = 0; i < p1.size(); i++) {
        p1[i]= temp[i];
    }//p1 is not bit reversed

    for (int i = 0; i < p1.size(); i++) {
        temp[i] = (p2[reverseBits(m_n, i)]);
    }//Reformats data in correct order

    for (int i = 0; i < p1.size(); i++) {
        p2[i] = temp[i];
    }//p2 is not bit reversed

    for (int i = 0; i < p2.size(); i++) {
        p2[i].imag = -p2[i].imag;
    }//p2 is now conjugate

    for (int i = 0; i < p1.size(); i++) {
        temp[i] = p1[i] * p2[i];
    }// temp = FFT(x) * conj(FFT(Y))

    for (int i = 0; i < p1.size(); i++) {
        out10 << temp[i].real.to_int() <<","<< temp[i].imag.to_int()<<endl;
    }// temp = FFT(x) * conj(FFT(Y))

    temp = ifft(temp);

    for (int i = 0; i < p1.size(); i++) {
        p1[reverseBits(p1.size(), i)].real = temp[i].real/p1.size();
        p1[reverseBits(p1.size(), i)].imag = temp[i].imag/p1.size();
    }//Reformats data in correct order

    return p1;
}

vector<FixedComplex<32> > XCorrelator::fft(vector<FixedComplex<32> > vals)
{
    vector<FixedComplex<32> > answers;
    int count = 0;
    filter_io_t data;

    data.type =  IO_TYPE_FIXED_COMPLEX_32;
    data.fc32 = FixedComplex<32>(0,0);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < m_n; j++) {
            data.fc32 = vals[j];
            m_fft->input(data);
            bool test = m_fft->output(data);
            if (test) {
                  answers.push_back(data.fc32);
            }//If an output is ready
        }//For every input
    }//run twice to get all outputs

    return answers;
}

vector<FixedComplex<32> > XCorrelator::ifft(vector<FixedComplex<32> > vals)
{
    vector<FixedComplex<32> > answers;
    int count = 0;
    filter_io_t data;
    data.type =  IO_TYPE_FIXED_COMPLEX_32;
    data.fc32 = FixedComplex<32>(0,0);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < m_n; j++) {
            data.fc32 = vals[j];
            m_ifft->input(data);
            bool test = m_ifft->output(data);
            if (test) {
                  answers.push_back(data.fc32);
            }//If an output is ready
        }//For every input
    }//run twice to get all output

    return answers;
}


XCorrelator::~XCorrelator()
{
    // TODO Auto-generated destructor stub
}

} /* namespace std */
