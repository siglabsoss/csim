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

template <size_t N, size_t M>
using FixedComplex2 = std::complex <sc_dt::sc_fixed <N, M> >;
