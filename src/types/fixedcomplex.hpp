/*
 * FixedComplex.h
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */

#ifndef FixedComplex_H_
#define FixedComplex_H_


#include <complex>
#include <types/fixedpoint.hpp>

template <size_t N, size_t M>
using FixedComplex2 = std::complex <FixedPoint <N, M> >;

typedef FixedPoint<16, 1>       FixedPointNorm16;
typedef FixedPoint<32, 2>       FixedPoint2_30;
typedef FixedPoint<32, 1>       FixedPoint1_31;

typedef FixedPoint<64, 34>      FixedPoint34_30;

typedef FixedComplex2<16, 1>    FixedComplex16;
typedef FixedComplex2<32, 17>   FixedComplex32; //XXX make this (32, 2) after revising the failing filters
typedef FixedComplex2<64, 49>   FixedComplex64;

typedef FixedComplex2<16, 1>    FixedComplexNorm16;
typedef FixedComplex2<32, 1>    FixedComplex1_31;
typedef FixedComplex2<32, 2>    FixedComplex2_30;

typedef std::complex<sc_fix>    FixedComplex;

class SLFixComplex
{
public:
    SLFixComplex();
    explicit SLFixComplex(size_t wordLength, size_t intLength);

    SLFixComplex operator+(const SLFixComplex &rhs);
    SLFixComplex operator-(const SLFixComplex &rhs);
    SLFixComplex operator*(const SLFixComplex &rhs);
    SLFixComplex operator*(const SLFixPoint &rhs);
    SLFixComplex operator/(const SLFixComplex &rhs);

    SLFixComplex &operator=(const SLFixComplex &rhs);
    SLFixComplex &operator=(double val);
    SLFixComplex &operator<<(size_t shift);
    SLFixComplex &operator>>(size_t shift);

    SLFixPoint  real() const;
    void        real(const SLFixPoint &val);
    void        real(double val);

    SLFixPoint  imag() const;
    void        imag(const SLFixPoint &val);
    void        imag(double val);

    void        setFormat(size_t wordLength, size_t intLength);
private:
    SLFixPoint m_real;
    SLFixPoint m_imag;
};

std::ostream& operator<<(std::ostream& os, const SLFixComplex& obj);

#endif /* FixedComplex_H_ */
