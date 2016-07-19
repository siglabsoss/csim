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
#include "fixedcomplex.h"

class fixedfir
{
public:

    int n; // Number of taps
    FixedComplex<16>* taps;

    fixedfir(int N, FixedComplex<16>* val);
    void fir(int length, FixedComplex<16>* input, FixedComplex<16>* output);
    virtual ~fixedfir();
};

#endif /* FIXEDFIR_H_ */
