/*
 * XCorrelator.hpp
 *
 *  Created on: Jul 28, 2016
 *      Author: ubuntu
 */

#ifndef XCorrelator_HPP_
#define XCorrelator_HPP_
#include <filters/fixedfft.hpp>
#include <filters/fixedifft.hpp>
#include <vector>
#include <utils/utils.hpp>
namespace std {

class XCorrelator
{
public:
    XCorrelator(int N);
    virtual ~XCorrelator();
    vector<FixedComplex<32> > xCorrolate(vector<FixedComplex<32> > x,vector<FixedComplex<32> > y);
    vector<FixedComplex<32> > fft(vector<FixedComplex<32> > vals);
    vector<FixedComplex<32> > ifft(vector<FixedComplex<32> > vals);
    int m_n;
    fixedfft* m_fft;
    fixedifft* m_ifft;

};

} /* namespace std */

#endif /* XCorrelator_HPP_ */
