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
    SLFixComplex(size_t                      wordLength,
                 ssize_t                     intLength,
                 SLFixPoint::quant_mode_t    quantMode,
                 SLFixPoint::overflow_mode_t overflowMode);
    SLFixComplex(const SLFixPoint::format_t& fmt);

    // Const operators, which return new objects
    SLFixComplex  operator+(const SLFixComplex& rhs) const;
    SLFixComplex  operator-(const SLFixComplex& rhs) const;
    SLFixComplex  operator*(const SLFixComplex& rhs) const;
    SLFixComplex  operator*(const SLFixPoint& rhs)   const;
    SLFixComplex  operator/(const SLFixComplex& rhs) const;

    // Self-mutating operators
    SLFixComplex& operator+=(const SLFixComplex& rhs);
    SLFixComplex& operator-=(const SLFixComplex& rhs);
    SLFixComplex& operator<<(size_t shift);
    SLFixComplex& operator>>(size_t shift);

    SLFixComplex& operator=(const SLFixComplex& rhs);
    SLFixComplex& operator=(double val);

    bool          operator==(const SLFixComplex& rhs) const;

    // Complex conjugation
    SLFixComplex  operator*();

    void          shiftRadixRight(size_t shiftAmount);
    void          shiftRadixLeft(size_t shiftAmount);

    SLFixPoint    real() const;
    void          real(const SLFixPoint& val);
    void          real(double val);

    SLFixPoint    imag() const;
    void          imag(const SLFixPoint& val);
    void          imag(double val);

    void          setFormat(size_t                      wordLength,
                            ssize_t                     intLength,
                            SLFixPoint::quant_mode_t    quantMode,
                            SLFixPoint::overflow_mode_t overflowMode);

    void                 setFormat(const SLFixPoint::format_t& fmt);

    SLFixPoint::format_t getFormat() const;

    void                 set(double real,
                             double imag);
    ComplexDouble        toComplexDouble() const;

    size_t               wl()  const; // word length
    ssize_t              iwl() const; // integer word length
    bool                 isFormatSet() const;

private:

    SLFixPoint m_real;
    SLFixPoint m_imag;
};

std::ostream& operator<<(std::ostream      & os,
                         const SLFixComplex& obj);

#endif /* FixedComplex_H_ */
