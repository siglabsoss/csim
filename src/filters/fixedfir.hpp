/*
 * fixedfir.h
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */

#ifndef FIXEDFIR_H_
#define FIXEDFIR_H_
#include<cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision


#include <types/fixedcomplex.hpp>
#include <core/filter_chain_element.hpp>

class fixedfir : public FilterChainElement
{
public:

    bool input(const filter_io_t &data) override;
    /**
     * output - provide an output sample to the caller.
     * @return false if no output sample is available.
     */
    bool output(filter_io_t &data) override;

    void tick() override;

    int                               m_n; // Number of taps
    std::vector< FixedComplex<16> >   m_taps;
    std::vector< FixedComplex<16> >   m_bench;

    FixedComplex<16>                  m_output;
    void reset();
    fixedfir(int N, FixedComplex<16>* tap);
    void fir(FixedComplex<16> &input);
    virtual ~fixedfir();
};

#endif /* FIXEDFIR_H_ */
