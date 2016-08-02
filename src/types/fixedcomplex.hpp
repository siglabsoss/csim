/*
 * FixedComplex.h
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */

#pragma once

#include <complex>

#define SC_INCLUDE_FX
#include <systemc.h>


template <size_t N>
using FixedComplex = std::complex <sc_dt::sc_fixed <N, 1> >;
/*
template <size_t N>
class FixedComplex : public std::complex <sc_dt::sc_fixed <N, 1> >
{
public:
    FixedComplex<16> to_16();
    FixedComplex<32> to_32();
    FixedComplex<64> to_64();
};
*/
