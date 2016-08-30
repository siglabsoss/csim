#pragma once

#include <core/filter_chain_element.hpp>

class Biquad : public FilterChainElement
{
public:
    Biquad();

    void init(const FixedComplex16 &b0,
            const FixedComplex16 &b1,
            const FixedComplex16 &b2,
            const FixedComplex16 &a1,
            const FixedComplex16 &a2,
            const FixedComplex16 &Gb,
            const FixedComplex16 &Ga);

    bool input(const filter_io_t &data) override;

    bool output(filter_io_t &data) override;

    void tick(void) override;

private:
    std::vector<FixedComplex16> m_x;
    std::vector<FixedComplex16> m_y;

    //Coefficients and gains
    std::vector<FixedComplex16> m_b;
    std::vector<FixedComplex16> m_a;
    FixedComplex16              m_Gb;
    FixedComplex16              m_Ga;

    bool                        m_newInput;
};
