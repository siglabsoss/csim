#pragma once

#include <cmath>
#include <vector>
#include <types/fixedpoint.hpp>

#define NCO_FP_FORMAT               NCO::TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE
#define NCO_PHASE_FORMAT            NCO::PWIDTH,  0, SLFixPoint::QUANT_TRUNCATE,    SLFixPoint::OVERFLOW_WRAP_AROUND

class NCO
{
public: //constants
    // In implementation it may be helpful to exploit half-wave and
    // quarter-wave symmetry to reduce storage requirements. In that
    // case a table of 1024 samples requires only 256 samples of storage.
    static constexpr int TBSIZE     = 1024; // TBSIZE of 4*8192 guarantees ~120 dB dynamic range
    static constexpr int LOG2TBSIZE = std::ceil(std::log(TBSIZE) / std::log(2));
    static constexpr int TBWIDTH    = 18;
    static constexpr int PWIDTH     = 32;
    static constexpr int TWOPIWIDTH = 18;
public:
    NCO(double freq);
    void pullNextSample(SLFixedPoint<NCO_FP_FORMAT> & cosine_out,
                        SLFixedPoint<NCO_FP_FORMAT> & sine_out);
private:
    // Digital Synthesis of Sinusoid
    std::vector < SLFixedPoint<NCO_FP_FORMAT> >      _cos;
    std::vector < SLFixedPoint<NCO_FP_FORMAT> >      _sin;
    SLFixedPoint<NCO_PHASE_FORMAT>                   _phase_acc;
    uint64_t                                         _dither_lfsr_memory;
    SLFixedPoint<NCO_PHASE_FORMAT>                   _phase_increment;
private:
};

