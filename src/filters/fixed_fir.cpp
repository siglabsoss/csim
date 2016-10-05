#include <filters/fixed_fir.hpp>
#include <iostream>

#include <core/logger.hpp>
#include <utils/utils.hpp>

FixedFIR::FixedFIR(std::vector<double> coeffs, unsigned coeffWidth) :
    FilterChainElement("FixedFIR"),
    m_coeffs(coeffs.size()),
    m_x(coeffs.size()),
    m_output(),
    m_accum()
{
    unsigned maxIntWidth = 0;
    double coeffArea = 0.0; // "coefficient area" defined as the sum of the magnitudes, used to determine accumulator width.
    //Find the number of integer bits needed to store the largest coefficient
    for (unsigned int i = 0; i < coeffs.size(); i++) {
        unsigned intWidth = utils::getIntegerBits(coeffs[i]);
        //std::cout << "intWidth = " << intWidth << "for " << coeffs[i] << std::endl;
        coeffArea += abs(coeffs[i]);
        if (intWidth > maxIntWidth) {
            maxIntWidth = intWidth;
        }
    }
    assert(maxIntWidth <= coeffWidth);

    //Construct the fixed point coefficients with scaling of 2^(coeffWidth - maxIntWidth)
    for (unsigned int i = 0; i < coeffs.size(); i++) {
        m_coeffs[i].setFormat(coeffWidth, maxIntWidth);
        m_coeffs[i] = coeffs[i];
    }

    //See http://www.digitalsignallabs.com/fir.pdf for analysis on FIR bit growth
    unsigned worstCaseBitGrowth = static_cast<unsigned>(ceil(log2(coeffArea)));
    //Construct the fixed point accumulator with enough width to avoid overflow
    unsigned bitsRequired = coeffWidth + 15 /*input width*/ + worstCaseBitGrowth;
    assert(bitsRequired < 52); //Lattice ECP5 max accumulator width

    m_accum.setFormat(bitsRequired, maxIntWidth + worstCaseBitGrowth);
    log_debug("FIR coefficient format is Q%d.%d, Accumulator format is Q%d.%d (max growth = %d)", maxIntWidth, coeffWidth - maxIntWidth, maxIntWidth + worstCaseBitGrowth, bitsRequired - (maxIntWidth + worstCaseBitGrowth), worstCaseBitGrowth);
}

bool FixedFIR::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_INT32_COMPLEX);
    SLFixComplex sample(16, 1);
    sample.real(data.intc.normalizedReal());
    sample.imag(data.intc.normalizedImag());

    m_output = filter(sample);
    return true;
}

bool FixedFIR::output(filter_io_t &data)
{
    data.type = IO_TYPE_INT32_COMPLEX;
    data.intc = m_output;
    return true;
}

void FixedFIR::tick()
{
}

ComplexInt FixedFIR::filter(SLFixComplex &input)
{
    m_accum = 0;
    m_x.push_front(input);

    for (unsigned int j = 0; (j < m_x.size()); j++) {
        m_accum = m_accum + (m_x[j] * m_coeffs[j]);
    } //Accumulate

    return accumToComplexInt();
}

ComplexInt FixedFIR::accumToComplexInt() const
{
    size_t accumWidth = m_accum.real().m_wl;
    int64_t real = m_accum.real().to_int64();
    int64_t imag = m_accum.imag().to_int64();
    //Take 16 MSBs XXX we should actually find the leading 1
    real >>= (accumWidth - (32 - 15));
    imag >>= (accumWidth - (32 - 15));
    ComplexInt result;
    result.c.real((real & 0xFFFF) << 16);
    result.c.imag((imag & 0xFFFF) << 16);
    result.exp = 0;
    return result;
}
