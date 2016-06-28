/*
 * fixedfir.h
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */
#include <stdio.h>
#define FILTER_LEN  63
#define MAX_INPUT_LEN   80// maximum length of filter than can be handled
#define MAX_FLT_LEN     1024// buffer to hold all of the input samples
#define BUFFER_LEN      (MAX_FLT_LEN - 1 + MAX_INPUT_LEN)
#ifndef FIXEDFIR_H_
#define FIXEDFIR_H_
#include <iostream>
#include<cstring>
#include "fixedcomplex.h"

class fixedfir {
public:
	
	FixedComplex<16> insamp[1024];
	int n; // Number of taps
	FixedComplex<16>* taps;
	FixedComplex<16> coeffs[ FILTER_LEN ];//Filter coeffs

	fixedfir(int N, FixedComplex<16>* val);
	void fir(int length, FixedComplex<16>* input, FixedComplex<16>* output);
	void init( void );
	virtual ~fixedfir();
	
	
};

#endif /* FIXEDFIR_H_ */
