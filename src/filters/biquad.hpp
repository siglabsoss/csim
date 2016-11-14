#pragma once

#include <core/filter_chain_element.hpp>
#include <vector>

#define BIQUAD_INPUT_FORMAT     16, 1, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

class Biquad : public FilterChainElement
{
public:
    struct SOSCoeffs {
        double b0;
        double b1;
        double b2;
        double a1;
        double a2;
    };
    Biquad(size_t coeffBitWidth);
    Biquad(const Biquad &other);

    void init(const SOSCoeffs &coeffs);

    bool input(const filter_io_t &data) override;

    bool output(filter_io_t &data) override;

    void tick(void) override;

private:
    std::vector<SLFixComplex> m_x;
    std::vector<SLFixComplex> m_y;

    //Coefficients and gains
    std::vector< SLFixPoint > m_b;
    std::vector< SLFixPoint > m_a;

    //Objects to store intermediate results (width calculated on initialization)
    SLFixComplex m_accum;

    bool                        m_newInput;
    size_t                      m_coeffWidth;
};
