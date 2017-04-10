/**
 * @file biquad.hpp
 * Implementation of a fixed-point biquadratic filter.
 */

#pragma once

#include <core/filter_chain_element.hpp>
#include <vector>

#define BIQUAD_INPUT_FORMAT     16, 1, SLFixPoint::QUANT_RND_HALF_UP, \
    SLFixPoint::OVERFLOW_SATURATE

/**
 * Implementation of a fixed-point biquadratic filter.
 *
 * See: https://en.wikipedia.org/wiki/Digital_biquad_filter for more
 * information.
 */
class Biquad : public FilterChainElement
{
public:

    /**
     * Represents the coefficients of the second-order-structure (SOS) filter.
     * The coefficients are intended to be normalized, such that a0 is assumed
     * to be 1.
     */
    struct SOSCoeffs
    {
        double b0;
        double b1;
        double b2;
        double a1;
        double a2;
    };

    /**
     * Construct a biquadratic filter
     * @param coeffBitWidth the number of integer bits used to store the given
     * coefficients.
     */
    Biquad(size_t coeffBitWidth);

    /**
     * Copy constructor.
     * Construct a new biquadratic filter, adopting the coefficient width from
     * the other filter.
     * @param other the other filter used as a reference to construct this one.
     */
    Biquad(const Biquad& other);

    /**
     * Initialize the filter using the given coefficients.
     * The number of integer and fraction bits for each of the coefficients
     * are determined based on the dynamic range of the given coefficents. The
     * number of integer and fraction bits of the accumulator are then derived.
     */
    void init(const SOSCoeffs& coeffs);

    /**
     * Feed in one input sample. Must be IO_TYPE_COMPLEX_FIXPOINT.
     * @param data one input sample of type IO_TYPE_COMPLEX_FIXPOINT.
     * @return true
     */
    bool input(const filter_io_t& data) override;

    /**
     * Output one sample.
     * @param data will store output sample of type IO_TYPE_COMPLEX_FIXPOINT.
     * @return true if sample was actually output.
     */
    bool output(filter_io_t& data) override;

    void tick(void) override;

private:

    std::vector<SLFixComplex> m_x;
    std::vector<SLFixComplex> m_y;

    // Coefficients and gains
    std::vector<SLFixPoint> m_b;
    std::vector<SLFixPoint> m_a;

    // Objects to store intermediate results (width calculated on
    // initialization)
    SLFixComplex m_accum;

    bool   m_newInput;
    size_t m_coeffWidth;
};
