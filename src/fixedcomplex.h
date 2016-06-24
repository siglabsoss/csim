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
	FixedComplex();
	bool operator==(const FixedComplex& rhs);
	bool operator!=(const FixedComplex& rhs);
	FixedComplex(sc_uint<16> rea);
	FixedComplex(sc_uint<16> rea, sc_uint<16> ima);
	FixedComplex operator+(const FixedComplex& rhs);
	FixedComplex operator-(const FixedComplex& rhs);
	FixedComplex operator*(const FixedComplex& rhs);
	FixedComplex operator/(const FixedComplex& rhs);
	FixedComplex operator<<(int shift);
	sc_uint<16> real;
	sc_uint<16> imag;

	virtual ~FixedComplex();
};

#endif /* FixedComplex_H_ */
