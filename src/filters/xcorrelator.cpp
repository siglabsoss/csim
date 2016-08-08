///*
// * XCorrelator.cpp
// *
// *  Created on: Jul 28, 2016
// *      Author: ubuntu
// */
//
//#include <filters/xcorrelator.hpp>
//
//using namespace std;
//
//XCorrelator::XCorrelator(int N)
//{
//    m_n = N;
//}
//
//vector<FixedComplex<32> > XCorrelator::xCorrelate(vector<FixedComplex<32> > x, vector<FixedComplex<32> > y)
//{
//
//    vector<FixedComplex<32> > p1 = fft(x);//output is FFT(x)
//    vector<FixedComplex<32> > p2 = fft(y);//output is FFT(y)
//    assert(p1.size() == m_n); //Same number of outputs into FFT as inputs
//    assert(p2.size() == m_n); //Same number of outputs into FFT as inputs
//
//    for (int i = 0; i < p2.size(); i++) {
//        p2[i].imag = -p2[i].imag;
//    }//conj(FFT(Y)))
//    FixedComplex<64> debug;
//    for (int i = 0; i < p1.size(); i++) {
//        debug = p1[i].to_64() * p2[i].to_64();
//        p1[i] = p1[i] * p2[i];
//        if (debug.real > p1[i].to_64().real) {
//            cout << "OVERFLOW" << endl;
//        }
//        if (debug.imag > p1[i].to_64().imag) {
//            cout << "OVERFLOW" << endl;
//        }
//    }//FFT(x) * conj(FFT(Y))
//
//    p1 = ifft(p1);// IFFT(FFT(x) * conj(FFT(Y)))
//
//    return fftshift(p1);
//}
//
//vector<FixedComplex<32> > XCorrelator::fft(vector<FixedComplex<32> > vals)
//{
//    fixedfft fft(m_n, 2);
//    vector<FixedComplex<32> > bitReversedAnswers;
//    vector<FixedComplex<32> > answers;
//    filter_io_t data;
//    data.type =  IO_TYPE_FIXED_COMPLEX_32;
//    data.fc32 = FixedComplex<32>(0,0);
//
//    for (int i = 0; i < 2; i++) {
//        for (int j = 0; j < m_n; j++) {
//            data.fc32 = vals[j];
//            fft.input(data);
//            bool test = fft.output(data);
//            if (test) {
//                bitReversedAnswers.push_back(data.fc32);
//            }//If an output is ready
//        }//For every input
//    }//run twice to get all outputs
//
//    for (int i = 0; i < m_n; i++) {
//             answers.push_back(bitReversedAnswers[reverseBits(m_n, i)]);
//     }//Reformats data in correct order
//
//    return answers;
//}
//
//vector<FixedComplex<32> > XCorrelator::ifft(vector<FixedComplex<32> > vals)
//{
//    fixedifft ifft(m_n);
//    vector<FixedComplex<32> > bitReversedAnswers;
//    vector<FixedComplex<32> > answers;
//    filter_io_t data;
//    data.type =  IO_TYPE_FIXED_COMPLEX_32;
//    data.fc32 = FixedComplex<32>(0,0);
//
//    for (int i = 0; i < 2; i++) {
//        for (int j = 0; j < m_n; j++) {
//            data.fc32 = vals[j];
//            ifft.input(data);
//            bool test = ifft.output(data);
//            if (test) {
//                bitReversedAnswers.push_back(data.fc32);
//            }//If an output is ready
//        }//For every input
//    }//run twice to get all outputs
//
//    for (int i = 0; i < m_n; i++) {
//             answers.push_back(bitReversedAnswers[reverseBits(m_n, i)]);
//     }//Reformats data in correct order
//
//    return answers;
//}
//
//
//vector<FixedComplex<32> > XCorrelator::fftshift(vector<FixedComplex<32> > vals)
//{
//    int size = vals.size() >> 1; //size/2
//    if (vals.size() % 2 == 1) {
//        size++; //Take ceiling
//    }
//
//    FixedComplex<32> temp;//to hold swap
//
//    for (int i = 0; i < size; i++) {
//        temp = vals[i];
//        vals[i] = vals[size+i];
//        vals[size+i] = temp;
//    }//Swaps front half and back half of vector
//    vals.erase(vals.begin());//Erase first value to match octave output
//    return vals;
//}//Swaps front half and back half of vector.
//
//
//
//
