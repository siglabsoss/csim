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
#include <types/complexdouble.hpp>

class SLFixComplex
{
public:
    SLFixComplex();
    SLFixComplex(size_t wordLength, size_t intLength);

    SLFixComplex operator+(const SLFixComplex &rhs);
    SLFixComplex operator-(const SLFixComplex &rhs);
    SLFixComplex operator*(const SLFixComplex &rhs);
    SLFixComplex operator*(const SLFixPoint &rhs);
    SLFixComplex operator/(const SLFixComplex &rhs);

    SLFixComplex &operator=(const SLFixComplex &rhs);
    SLFixComplex &operator=(double val);
    SLFixComplex &operator<<(size_t shift);
    SLFixComplex &operator>>(size_t shift);

    void        shiftRadixRight(size_t shiftAmount);
    void        shiftRadixLeft(size_t shiftAmount);

    SLFixPoint  real() const;
    void        real(const SLFixPoint &val);
    void        real(double val);

    SLFixPoint  imag() const;
    void        imag(const SLFixPoint &val);
    void        imag(double val);

    void        setFormat(size_t wordLength, size_t intLength);
    void        setFormat(const SLFixComplex &other);

    void        set(double real, double imag);
    ComplexDouble toComplexDouble() const;
private:
    SLFixPoint m_real;
    SLFixPoint m_imag;
};

std::ostream& operator<<(std::ostream& os, const SLFixComplex& obj);

#endif /* FixedComplex_H_ */
