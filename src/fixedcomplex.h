/*
 * FixedComplex.h
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */



#ifndef FixedComplex_H_
#define FixedComplex_H_

#include "systemc.h"

class FixedComplex {

private:

public:

	sc_int<16> real;
	sc_int<16> imag;

	FixedComplex();
	bool operator==(const FixedComplex& rhs);
	bool operator!=(const FixedComplex& rhs);
	FixedComplex(sc_int<16> rea);
	FixedComplex(sc_int<16> rea, sc_int<16> ima);
	FixedComplex operator+(const FixedComplex& rhs);
	FixedComplex operator-(const FixedComplex& rhs);
	FixedComplex operator*(const FixedComplex& rhs);
	FixedComplex operator/(const FixedComplex& rhs);
	FixedComplex operator<<(int shift);
	FixedComplex operator>>(int shift);
	virtual ~FixedComplex();
};

#endif /* FixedComplex_H_ */
