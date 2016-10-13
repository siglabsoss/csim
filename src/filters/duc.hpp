#pragma once

#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>

#include <types/filter_io.hpp>
#include <core/filter_chain_element.hpp>
#include <filters/nco.hpp>
#include <filters/fixed_fir.hpp>

#define DUC_OUTPUT_FP_FORMAT            16,     2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE
#define DUC_INPUT_FP_FORMAT             18,     2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

#define UP2_COEFF_FP_FORMAT             18,  1, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE
#define UP5_COEFF_FP_FORMAT             18,  2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

class DigitalUpConverter : public FilterChainElement
{
public:
    DigitalUpConverter(double freq, const std::vector<double> &up2Coeffs, const std::vector<double> &up5Coeffs);

    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data)      override;
    void tick(void)                     override;

private: //methods
    bool push(
        // Inputs
        const SLFixedPoint<DUC_INPUT_FP_FORMAT> & inph_in,
        const SLFixedPoint<DUC_INPUT_FP_FORMAT> & quad_in,
        // Outputs
        SLFixedPoint<DUC_INPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DUC_INPUT_FP_FORMAT> & quad_out);

private: //members

    NCO                                                    _nco;
    FixedFIR *                                             _up2FIR;
    FixedFIR *                                             _up5FIR;

    // Output variables
    bool                                                   _output_ready;
    SLFixedPoint<DUC_OUTPUT_FP_FORMAT>                     _output_inph;
    SLFixedPoint<DUC_OUTPUT_FP_FORMAT>                     _output_quad;

    bool                                                   _got_input;
    SLFixedPoint<DUC_INPUT_FP_FORMAT>                      _inph_in;
    SLFixedPoint<DUC_INPUT_FP_FORMAT>                      _quad_in;
    unsigned long                                          _iteration;
};

