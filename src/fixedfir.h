/*
 * fixedfir.h
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */
#include <stdio.h>

#ifndef FIXEDFIR_H_
#define FIXEDFIR_H_
#include <iostream>
#include<cstring>
#include "fixedcomplex.h"

class fixedfir {
public:
	

	int n; // Number of taps
	FixedComplex<16>* taps;


	fixedfir(int N, FixedComplex<16>* val);
	void fir(int length, FixedComplex<16>* input, FixedComplex<16>* output);

	virtual ~fixedfir();
	
	
};

#endif /* FIXEDFIR_H_ */
