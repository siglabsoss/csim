/*
 * fixedcomplex.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */

#include <types/fixedcomplex.hpp>

template<int B>
void FixedComplex<B>::print()
{
    cout << to_string() << endl;
}

template<int B>
std::string FixedComplex<B>::to_string()
{
    std::ostringstream oss;
    oss << "Real: " << this->real << " " << "Imaginary: " << this->imag;
    return oss.str();
}

template<int B>
FixedComplex<B>::FixedComplex()
{
    this->real = 0;
    this->imag = 0;
} //Default constructor

template<int B>
FixedComplex<B>::FixedComplex(sc_int<B> rea)
{
    this->real = rea;
    this->imag = 0;
} //Constructor with only real arg

template<int B>
FixedComplex<B>::FixedComplex(sc_int<B> rea, sc_int<B> ima)
{

    this->real = rea;
    this->imag = ima;
} //Constructor with real and imaginary args

template<int B>
FixedComplex<B> FixedComplex<B>::operator+(const FixedComplex& rhs)
{
    FixedComplex c;
    c.real = this->real + rhs.real;
    c.imag = this->imag + rhs.imag;
    return c;
} //Addition

template<int B>
FixedComplex<B> FixedComplex<B>::operator-(const FixedComplex& rhs)
{
    FixedComplex c;
    c.real = this->real - rhs.real;
    c.imag = this->imag - rhs.imag;
    return c;
} //Subtraction

template<int B>
FixedComplex<B> FixedComplex<B>::operator*(const FixedComplex& rhs)
{
    FixedComplex c;
    c.real = (this->real * rhs.real) - (this->imag * rhs.imag); //Adds real parts
    c.imag = (this->real * rhs.imag) + (this->imag * rhs.real); //Adds imaginary parts
    return c;
} //Multiplication

template<int B>
FixedComplex<B> FixedComplex<B>::operator/(const FixedComplex& rhs)
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
    num.real = num.real / denom.real;
    num.imag = (num.imag / denom.real);
    return num;

//	FixedComplex d = rhs;
//	c = *this;
//	//num = c * conj;
//	denom = d * conj;
//	num.real = (c.real*conj.real/denom.real) - (c.imag*conj.imag/denom.real);
//	num.imag = (c.imag*conj.real/denom.real) + (conj.imag*c.real/denom.real);

} //Division. Multiplies by conjugate and simplifies

template<int B>
FixedComplex<B> FixedComplex<B>::operator<<(int shift)
{
    FixedComplex c;
    c.real = this->real << shift;
    c.imag = this->imag << shift;
    return c;
} //Left Shift

template<int B>
FixedComplex<B> FixedComplex<B>::operator>>(int shift)
{
    FixedComplex c;
    c.real = this->real >> shift;
    c.imag = this->imag >> shift;
    return c;
} //Right Shift

template<int B>
bool FixedComplex<B>::operator==(const FixedComplex& rhs)
{

    if ((this->real == rhs.real) && (this->imag == rhs.imag))
        return true;
    else
        return false;

} // == operator

template<int B>
bool FixedComplex<B>::operator!=(const FixedComplex& rhs)
{
    if ((this->real == rhs.real) && (this->imag == rhs.imag))
        return false;
    else
        return true;
} // != operator

template<int B>
FixedComplex<B>::~FixedComplex()
{

}

template<int B>
FixedComplex<16> FixedComplex<B>::to_16()
{
    FixedComplex<16> c;
    c.real = this->real;
    c.imag = this->imag;
    return c;
}

template<int B>
FixedComplex<32> FixedComplex<B>::to_32()
{
    FixedComplex<32> c;
    c.real = this->real;
    c.imag = this->imag;
    return c;
}

template class FixedComplex<2> ;
template class FixedComplex<3> ;
template class FixedComplex<4> ;
template class FixedComplex<5> ;
template class FixedComplex<6> ;
template class FixedComplex<7> ;
template class FixedComplex<8> ;
template class FixedComplex<9> ;
template class FixedComplex<10> ;
template class FixedComplex<11> ;
template class FixedComplex<12> ;
template class FixedComplex<13> ;
template class FixedComplex<14> ;
template class FixedComplex<15> ;
template class FixedComplex<16> ;
template class FixedComplex<17> ;
template class FixedComplex<18> ;
template class FixedComplex<19> ;
template class FixedComplex<20> ;
template class FixedComplex<21> ;
template class FixedComplex<22> ;
template class FixedComplex<23> ;
template class FixedComplex<24> ;
template class FixedComplex<25> ;
template class FixedComplex<26> ;
template class FixedComplex<27> ;
template class FixedComplex<28> ;
template class FixedComplex<29> ;
template class FixedComplex<30> ;
template class FixedComplex<31> ;
template class FixedComplex<32> ;

