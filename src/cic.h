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

		int numBRegisters;
		int numARegisters;
		FixedComplex<32>* a;
		FixedComplex<32>* b;

		int r;
		int sample;
		fixedcic(int R);
		void cic(int length, FixedComplex<16>* input, FixedComplex<16>* output);
		FixedComplex<32> integrate(FixedComplex<16> current);
		FixedComplex<32> comb(FixedComplex<32> current);
		bool downsample();
		virtual ~fixedcic();
};




#endif /* CIC_H_ */
