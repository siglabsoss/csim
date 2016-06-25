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
}//Default constructor


FixedComplex::FixedComplex(sc_int<16> rea)
{
	this->real = rea;
	this->imag = 0;
}//Constructor with only real arg


FixedComplex::FixedComplex(sc_int<16> rea, sc_int<16> ima) {

	this->real = rea;
	this->imag = ima;
}//Constructor with real and imaginary args

FixedComplex FixedComplex::operator+(const FixedComplex& rhs)
{
	FixedComplex c;
	c.real = this->real + rhs.real;
	c.imag = this->imag + rhs.imag;
	return c;
}//Addition


FixedComplex FixedComplex::operator-(const FixedComplex& rhs)
{
	FixedComplex c;
	c.real = this->real - rhs.real;
	c.imag = this->imag - rhs.imag;
	return c;
}//Subtraction

FixedComplex FixedComplex::operator*(const FixedComplex& rhs)
{
	FixedComplex c;
	c.real = (this->real * rhs.real) - (this->imag * rhs.imag);//Adds real parts
	c.imag = (this->real * rhs.imag) + (this->imag * rhs.real);//Adds imaginary parts
	return c;
}//Multiplication


FixedComplex FixedComplex::operator/(const FixedComplex& rhs)
{
	FixedComplex num;
	FixedComplex denom;
	FixedComplex conj;
	conj.real = rhs.real;
	conj.imag = -rhs.imag;
	FixedComplex c;
	c = *this;
	num = c * conj;
	FixedComplex d = rhs;
	denom = d * conj;
	num.real = num.real/denom.real;
	num.imag = (num.imag/denom.real);
	return num;
}//Division. Multiplies by conjugate and simplifies


FixedComplex FixedComplex::operator<<(int shift)
{
	FixedComplex c;
	c.real = this->real<<shift;
	c.imag = this->imag<<shift;
	return c;
}//Left Shift


FixedComplex FixedComplex::operator>>(int shift)
{
	FixedComplex c;
	c.real = this->real>>shift;
	c.imag = this->imag>>shift;
	return c;
}//Right Shift


bool FixedComplex::operator==(const FixedComplex& rhs)
{

	if((this->real == rhs.real) && (this->imag == rhs.imag))
		return true;
	else
		return false;

}// == operator

bool FixedComplex::operator!=(const FixedComplex& rhs)
{
	if((this->real == rhs.real) && (this->imag == rhs.imag))
		return false;
	else
		return true;
}// != operator


FixedComplex::~FixedComplex() {

}

