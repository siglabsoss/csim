#pragma once

#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>

#include <types/filter_io.hpp>
#include <core/filter_chain_element.hpp>

// In implementation it may be helpful to exploit half-wave and
// quarter-wave symmetry to reduce storage requirements. In that
// case a table of 1024 samples requires only 256 samples of storage.
static constexpr int TBSIZE     = 1024; // TBSIZE of 4*8192 guarantees ~120 dB dynamic range
static constexpr int LOG2TBSIZE = std::ceil(std::log(TBSIZE) / std::log(2));
static constexpr int TBWIDTH    = 18;
static constexpr int PWIDTH     = 32;
static constexpr int INWIDTH    = 16;
static constexpr int OUTWIDTH   = 18;

class DigitalDownConverter : public FilterChainElement
{
public:
    DigitalDownConverter(double freq, const std::vector<double> &halfbandCoeffs, const std::vector<double> &by5Coeffs);

    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data)      override;
    void tick(void)                     override;

private: //methods
    bool push(
        // Inputs
        const SLFixedPoint<INWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & input_sample,
        // Outputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_out,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_out);
    bool pull_sinusoid(
        // Outputs
        SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & cosine_out,
        SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & sine_out);
    bool push_halfband(
        // Inputs
        const SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  & inph_in,
        const SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  & quad_in,
        // Outputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_out,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_out);
    bool push_by5(
        // Inputs
        const SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_in,
        const SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_in,
        // Outputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_out,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_out);

private: //members
    // Digital Synthesis of Sinusoid
    std::vector < SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> >      _cos;//[TBSIZE];
    std::vector < SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> >      _sin;//[TBSIZE];
    SLFixedPoint<PWIDTH, 0,  SLFixPoint::QUANT_TRUNCATE,    SLFixPoint::OVERFLOW_WRAP_AROUND>   _phase_acc;
    unsigned long                                                                               _dither_lfsr_memory;
    SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>       _phase_increment;

    // Decimation Filter (by 2)
    std::vector < SLFixedPoint<18, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> >          _halfband_coeffs;//[HB_LENGTH];
    std::vector < SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> >    _halfband_inph_delays;//[HB_LENGTH];
    std::vector < SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> >    _halfband_quad_delays;//[HB_LENGTH];
    unsigned long                                                                                              _halfband_iteration;

    // Decimation Filter (by 5)
    std::vector < SLFixedPoint<18, -1, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> >         _by5_coeffs;//[B5_LENGTH];
    std::vector < SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> >    _by5_inph_delays;//[B5_LENGTH];
    std::vector < SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> >    _by5_quad_delays;//[B5_LENGTH];
    unsigned long                                                                                              _by5_iteration;

    // Output variables
    bool                                                                                                       _output_ready;
    SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>                    _output_inph;
    SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>                    _output_quad;
};

