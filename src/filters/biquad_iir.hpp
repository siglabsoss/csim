/**
 * @file biquad_iir.hpp
 * Implementation of a fixed-point IIR filter composed of one or more
 * biquadratic structures.
 */
#pragma once

#include <core/filter_chain_element.hpp>
#include <filters/biquad.hpp>

/**
 * Implementation of a fixed-point IIR filter composed of one or more
 * biquadratic structures.
 * @see Biquad
 */
class BiquadIIR : public FilterChainElement
{
public:

    /**
     * Construct the filter.
     * @param numStages number of cascaded biquad structures.
     */
    BiquadIIR(size_t numStages);

    bool input(const filter_io_t& data) override;

    bool output(filter_io_t& data) override;

    void tick(void) override;

    /**
     * Initialize the filter with the given coefficients.
     * @param coeffs a vector of SOSCoeffs structures. The length of the
     * vector must equal the number of stages the filter was constructed for.
     */
    void init(const std::vector<Biquad::SOSCoeffs>& coeffs);

private:

    std::vector<Biquad> m_stages;
    bool m_outputReady;
    filter_io_t m_sample;
};
