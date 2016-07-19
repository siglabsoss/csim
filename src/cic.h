/*
 * cic.h
 *
 *  Created on: Jul 7, 2016
 *      Author: ubuntu
 */

#ifndef CIC_H_
#define CIC_H_
#include<cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include "fixedcomplex.h"

class fixedcic
{
public:

    int numBRegisters; //number of b registers
    int numARegisters; //number of a registers
    FixedComplex<32>* a; //a registers
    FixedComplex<32>* b; //b registers

    int r; //Decimation factor
    int samples; //How many samples have been processed.
    fixedcic(int R, int aregs, int bregs); //Contructor takes in decimation factor and number of regs on each side.
    int cic(int length, FixedComplex<16>* input, FixedComplex<16>* output); //actual filtering of data. Returns number of outputs
    FixedComplex<32> integrate(FixedComplex<16> current); //integrate side of filter
    FixedComplex<32> comb(FixedComplex<32> current); //comb side of filter
    bool downsample(); //returns false when the sample should be passed
    virtual ~fixedcic();
};

#endif /* CIC_H_ */
