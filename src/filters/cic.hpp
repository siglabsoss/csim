/*
 * cic.h
 *
 *  Created on: Jul 7, 2016
 *      Author: ubuntu
 */

#ifndef CIC_H_
#define CIC_H_

#include <stdio.h>
#include <iostream>
#include <vector>

#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>

class fixedcic : public FilterChainElement
{
public:

    bool input(const filter_io_t &data) override;
    /**
     * output - provide an output sample to the caller.
     * @return false if no output sample is available.
     */
    bool output(filter_io_t &data) override;

    void tick() override;
    fixedcic();
    fixedcic(int R, int aregs, int bregs); //Constructor takes in decimation factor and number of regs on each side.
    void cic(FixedComplex2<16, 1> &input); //actual filtering of data. Returns number of outputs
    FixedComplex2<16, 1> integrate(FixedComplex2<16, 1> current); //integrate side of filter
    FixedComplex2<16, 1> comb(FixedComplex2<16, 1> current); //comb side of filter
    bool downsample(); //returns false when the sample should be passed
    void reset();

    bool goodOutput;
    int                         m_numBRegisters; //number of b registers
    int                         m_numARegisters; //number of a registers
    int                         m_r; //Decimation factor
    int                         m_samples; //How many samples have been processed
    std::vector<FixedComplex2<16, 1> >   m_a; //a registers
    std::vector<FixedComplex2<16, 1> >   m_b; //b registers
    FixedComplex2<16, 1>            m_output;
};

#endif /* CIC_H_ */
