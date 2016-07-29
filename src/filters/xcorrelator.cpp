/*
 * XCorrelator.cpp
 *
 *  Created on: Jul 28, 2016
 *      Author: ubuntu
 */

#include <XCorrelator.hpp>

namespace std {

XCorrelator::XCorrelator(int N)
{
    m_n = N;
    m_fft = new fixedfft(N*2);
    m_ifft = new fixedifft(N*2);
}

vector<FixedComplex<32> > XCorrelator::xCorrolate(vector<FixedComplex<32> > x, vector<FixedComplex<32> > y)
{
    vector<FixedComplex<32> > p1 = fft(x);
    vector<FixedComplex<32> > p2 = fft(y);
    vector<FixedComplex<32> > temp;

    for (int i = 0; i < p1.size(); i++) {
            temp[reverseBits(m_n*2, i)] = p1[i];
        }//Reformats data in correct order


    for (int i = 0; i < p1.size(); i++) {
        cout << temp[i];
    }

    for (int i; i < p1.size(); i++) {
        temp[i] = p1[i] * p2[i];
    }

    for (int i = 0; i < p1.size(); i++) {
        p1[reverseBits(m_n*2, i)] = temp[i];
    }//Reformats data in correct order

    p1 = ifft(p1);

    for (int i; i < p1.size(); i++) {
        cout << p1[i];
    }

}

vector<FixedComplex<32> > XCorrelator::fft(vector<FixedComplex<32> > vals)
{
    vector<FixedComplex<32> > answers;
    int count = 0;
        filter_io_t data;
        data.type =  IO_TYPE_FIXED_COMPLEX_32;
        for (int j = m_n; j < 2*m_n; j++) {
            vals.push_back(FixedComplex<32>(0,0));

        }//zero pads

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2*m_n; j++) {

                data.fc32 = vals[j];
                m_fft->input(data);
                bool test = m_fft->output(data);
                if (test) {
                      answers.push_back(data.fc32);
                }
            }
        }

        return answers;

}

vector<FixedComplex<32> > XCorrelator::ifft(vector<FixedComplex<32> > vals)
{
    vector<FixedComplex<32> > answers;
    int count = 0;
        filter_io_t data;
        data.type =  IO_TYPE_FIXED_COMPLEX_32;
        for (int j = m_n; j < 2*m_n; j++) {
            vals.push_back(FixedComplex<32>(0,0));

        }//zero pads

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2*m_n; j++) {

                data.fc32 = vals[j];
                m_fft->input(data);
                bool test = m_ifft->output(data);
                if (test) {
                      answers.push_back(data.fc32);
                }
            }
        }
        return answers;
}


XCorrelator::~XCorrelator()
{
    // TODO Auto-generated destructor stub
}

} /* namespace std */