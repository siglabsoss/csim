/*
 * fixedcomplex.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */

#include <fixedcomplex.h>

FixedComplex::FixedComplex()
{
	this->real = 0;
	this->imag = 0;
}

FixedComplex::FixedComplex(sc_uint<16> rea)
{
	this->real = rea;
	this->imag = 0;
}


FixedComplex::FixedComplex(sc_uint<16> rea, sc_uint<16> ima) {

	this->real = rea;
	this->imag = ima;
	// TODO Auto-generated constructor stub

}

FixedComplex FixedComplex::operator+(const FixedComplex& rhs)
{
	FixedComplex c;
	c.real = this->real + rhs.real;
	c.imag = this->imag + rhs.imag;
	return c;

}


FixedComplex FixedComplex::operator-(const FixedComplex& rhs)
{
	FixedComplex c;
	c.real = this->real - rhs.real;
	c.imag = this->imag - rhs.imag;
	return c;

}

FixedComplex FixedComplex::operator*(const FixedComplex& rhs)
{
	FixedComplex c;
	c.real = (this->real * rhs.real) - (this->imag * rhs.imag);
	c.imag = (this->real * rhs.imag) + (this->imag * rhs.real);
	return c;

}


FixedComplex FixedComplex::operator/(const FixedComplex& rhs)
{
	FixedComplex c;
	return c;
}

FixedComplex FixedComplex::operator<<(int shift)
{
	this->real = this->real*2;
}//Left Shift


bool FixedComplex::operator==(const FixedComplex& rhs)
{

	if((this->real == rhs.real) && (this->imag == rhs.imag))
		return true;
	else
		return false;

}

bool FixedComplex::operator!=(const FixedComplex& rhs)
{
	if((this->real == rhs.real) && (this->imag == rhs.imag))
		return false;
	else
		return true;
}


FixedComplex::~FixedComplex() {
	// TODO Auto-generated destructor stub
}

