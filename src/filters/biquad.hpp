#pragma once

#include <core/filter_chain_element.hpp>
#include <memory>

class Biquad : public FilterChainElement
{
public:
    Biquad();

    void init(double b0,
            double b1,
            double b2,
            double a1,
            double a2);

    bool input(const filter_io_t &data) override;

    bool output(filter_io_t &data) override;

    void tick(void) override;

private:
    static void complexScalarMultiply(std::complex<sc_fix> &result, std::complex<sc_fix> &complex, sc_fix &scalar);
    static void shiftRightFixedComplex(std::complex<sc_fix> &val, size_t shiftBits);

private:
    std::vector<FixedComplexNorm16> m_x;
    std::vector<FixedComplexNorm16> m_y;

    //Coefficients and gains
    std::vector< std::pair<std::unique_ptr<sc_fix>, size_t > > m_b;
    std::vector< std::pair<std::unique_ptr<sc_fix>, size_t > > m_a;
    bool                        m_newInput;
};
