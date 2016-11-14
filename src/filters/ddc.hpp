#pragma once

#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>

#include <types/filter_io.hpp>
#include <core/filter_chain_element.hpp>
#include <filters/nco.hpp>
#include <filters/fixed_fir.hpp>

#define DDC_INPUT_WL   16
#define DDC_INPUT_IWL  2
#define DDC_OUTPUT_WL  22
#define DDC_OUTPUT_IWL 2

#define DDC_OUTPUT_FP_FORMAT            DDC_OUTPUT_WL,     DDC_OUTPUT_IWL, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE
#define DDC_INPUT_FP_FORMAT             DDC_INPUT_WL,      DDC_INPUT_IWL,  SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

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
        const SLFixedPoint<DDC_INPUT_FP_FORMAT> & input_sample,
        // Outputs
        SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & quad_out);

private: //members

    NCO                                                    _nco;
    FixedFIR *                                             _halfbandFIR;
    FixedFIR *                                             _by5FIR;
    SLFixedPoint<DDC_INPUT_FP_FORMAT>                      _input;
    bool                                                   _did_receive_input;

    // Output variables
    bool                                                   _output_ready;
    SLFixedPoint<DDC_OUTPUT_FP_FORMAT>                     _output_inph;
    SLFixedPoint<DDC_OUTPUT_FP_FORMAT>                     _output_quad;
};

