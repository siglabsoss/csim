/*
 * FixedComplex.h
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */



#ifndef FixedComplex_H_
#define FixedComplex_H_

#include "systemc.h"
#include <iostream>
template <int B>
class FixedComplex {

private:

public:

	sc_int<B> real;
	sc_int<B> imag;

	FixedComplex();
	bool operator==(const FixedComplex& rhs);
	bool operator!=(const FixedComplex& rhs);
	FixedComplex(sc_int<B> rea);
	FixedComplex(sc_int<B> rea, sc_int<B> ima);
	void print();
	FixedComplex operator+(const FixedComplex& rhs);
	FixedComplex operator-(const FixedComplex& rhs);
	FixedComplex operator*(const FixedComplex& rhs);
	FixedComplex operator/(const FixedComplex& rhs);
	FixedComplex operator<<(int shift);
	FixedComplex operator>>(int shift);
	FixedComplex<16> to_16();
	FixedComplex<32> to_32();
	virtual ~FixedComplex();
};

#endif /* FixedComplex_H_ */
