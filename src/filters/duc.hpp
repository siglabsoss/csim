#pragma once

#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>

#include <types/filter_io.hpp>
#include <core/filter_chain_element.hpp>
#include <filters/nco.hpp>

static constexpr int INWIDTH    = 16;
static constexpr int OUTWIDTH   = 18;

#define DUC_OUTPUT_FP_FORMAT            16,     2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE
#define DUC_INPUT_FP_FORMAT             18,     2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

#define UP2_COEFF_FP_FORMAT             18,  1, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE
#define UP5_COEFF_FP_FORMAT             18,  2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

class DigitalUpConverter : public FilterChainElement
{
public:
    DigitalUpConverter(double freq, const std::vector<double> &by2Coeffs, const std::vector<double> &by5Coeffs);

    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data)      override;
    void tick(void)                     override;

private: //methods
    bool push(
        // Inputs
        const SLFixedPoint<DUC_INPUT_FP_FORMAT> & inph_in,
        const SLFixedPoint<DUC_INPUT_FP_FORMAT> & quad_in,
        // Outputs
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & quad_out);
    bool push_up2(
        // Inputs
        const SLFixedPoint<DUC_OUTPUT_FP_FORMAT>  & inph_in,
        const SLFixedPoint<DUC_OUTPUT_FP_FORMAT>  & quad_in,
        // Outputs
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & quad_out);
    bool push_up5(
        // Inputs
        const SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & inph_in,
        const SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & quad_in,
        // Outputs
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & quad_out);

private: //members

    NCO                                                    _nco;

    // Interpolation Filter (by 2)
    std::vector < SLFixedPoint<UP2_COEFF_FP_FORMAT> >      _up2_coeffs;
    std::vector < SLFixedPoint<DUC_OUTPUT_FP_FORMAT> >     _up2_inph_delays;
    std::vector < SLFixedPoint<DUC_OUTPUT_FP_FORMAT> >     _up2_quad_delays;

    // Interpolation Filter (by 5)
    std::vector < SLFixedPoint<UP5_COEFF_FP_FORMAT> >      _up5_coeffs;
    std::vector < SLFixedPoint<DUC_OUTPUT_FP_FORMAT> >     _up5_inph_delays;
    std::vector < SLFixedPoint<DUC_OUTPUT_FP_FORMAT> >     _up5_quad_delays;

    // Output variables
    bool                                                   _output_ready;
    SLFixedPoint<DUC_OUTPUT_FP_FORMAT>                     _output_inph;
    SLFixedPoint<DUC_OUTPUT_FP_FORMAT>                     _output_quad;

    bool                                                   _got_input;
    SLFixedPoint<DUC_INPUT_FP_FORMAT>                      _inph_in;
    SLFixedPoint<DUC_INPUT_FP_FORMAT>                      _quad_in;
    unsigned long                                          _iteration;
};

