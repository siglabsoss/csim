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
#include "filter_chain_element.hpp"
#include "fixedcomplex.h"

class fixedcic : public FilterChainElement
{
public:

    bool input(const block_io_t &data) override;
    /**
     * output - provide an output sample to the caller.
     * @return false if no output sample is available.
     */
    bool output(block_io_t &data) override;

    void tick() override;
    fixedcic();
    fixedcic(int R, int aregs, int bregs); //Constructor takes in decimation factor and number of regs on each side.
    void cic(FixedComplex<16> &input); //actual filtering of data. Returns number of outputs
    FixedComplex<32> integrate(FixedComplex<16> current); //integrate side of filter
    FixedComplex<32> comb(FixedComplex<32> current); //comb side of filter
    bool downsample(); //returns false when the sample should be passed
    void reset();
    int                         m_numBRegisters; //number of b registers
    int                         m_numARegisters; //number of a registers
    int                         m_r; //Decimation factor
    int                         m_samples; //How many samples have been processed
    vector<FixedComplex<32> >   m_a; //a registers
    vector<FixedComplex<32> >   m_b; //b registers
    FixedComplex<16>            m_output;
};

#endif /* CIC_H_ */
