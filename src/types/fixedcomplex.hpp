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
typedef FixedPoint<32, 1>       FixedPointNorm32;

typedef FixedComplex2<16, 1> FixedComplex16;
typedef FixedComplex2<32, 17> FixedComplex32;
typedef FixedComplex2<64, 49> FixedComplex64;

typedef FixedComplex2<16, 1>    FixedComplexNorm16;
typedef FixedComplex2<32, 1>    FixedComplexNorm32;

typedef FixedComplex2<16, 1>    FixedComplex16;
typedef FixedComplex2<64, 49>   FixedComplex32;
typedef FixedComplex2<64, 48>   FixedComplex64;

typedef std::complex<sc_fix>    FixedComplex;

#endif /* FixedComplex_H_ */
