#pragma once

#define SC_INCLUDE_FX
#include <systemc.h>

template <size_t N, size_t M>
using FixedPoint = sc_dt::sc_fixed <N, M, SC_RND, SC_SAT>;

typedef FixedPoint<32, 4> FixedPoint32;
