#pragma once

#include <core/filter_chain_element.hpp>
#include <memory>

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
    static void complexScalarMultiply(std::complex<sc_fix> &result, const std::complex<sc_fix> &complex, const sc_fix &scalar);
    static void shiftRightFixedComplex(std::complex<sc_fix> &val, size_t shiftBits);

private:
    std::vector<FixedComplexNorm16> m_x;
    std::vector<FixedComplexNorm16> m_y;

    //Coefficients and gains
    std::vector< std::pair<std::unique_ptr<sc_fix>, size_t > > m_b;
    std::vector< std::pair<std::unique_ptr<sc_fix>, size_t > > m_a;

    //Objects to store intermediate results (width calculated on initialization)
    std::unique_ptr< std::complex<sc_fix> > m_bx0;
    std::unique_ptr< std::complex<sc_fix> > m_bx1;
    std::unique_ptr< std::complex<sc_fix> > m_bx2;
    std::unique_ptr< std::complex<sc_fix> > m_ay1;
    std::unique_ptr< std::complex<sc_fix> > m_ay2;

    bool                        m_newInput;
    size_t                      m_coeffWidth;
};
