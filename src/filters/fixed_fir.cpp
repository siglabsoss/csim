#include <filters/fixed_fir.hpp>
#include <iostream>

#include <core/logger.hpp>
#include <utils/utils.hpp>

FixedFIR::FixedFIR(std::vector<double> coeffs, Config conf) :
    FilterChainElement("FixedFIR"),
    m_coeffs(coeffs.size()),
    m_x(coeffs.size(), SLFixComplex(conf.wlDelay, conf.iwlDelay)),
    m_output(conf.wlOut, conf.iwlOut),
    m_accum(),
    m_rateAdj(conf.rateChange),
    m_iteration(0)
{
    unsigned maxIntWidth = 0;
    double coeffArea = 0.0; // "coefficient area" defined as the sum of the magnitudes, used to determine accumulator width.
    //Find the number of integer bits needed to store the largest coefficient
    for (unsigned int i = 0; i < coeffs.size(); i++) {
        unsigned intWidth = utils::getIntegerBits(coeffs[i]);
        //std::cout << "intWidth = " << intWidth << "for " << coeffs[i] << std::endl;
        coeffArea += fabs(coeffs[i]);
        if (intWidth > maxIntWidth) {
            maxIntWidth = intWidth;
        }
    }
    assert(maxIntWidth <= conf.wlCoeff);

    //Construct the fixed point coefficients with scaling of 2^(wlCoeff - maxIntWidth)
    for (unsigned int i = 0; i < coeffs.size(); i++) {
        m_coeffs[i].setFormat(conf.wlCoeff, maxIntWidth, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
        m_coeffs[i] = coeffs[i];
    }

    //See http://www.digitalsignallabs.com/fir.pdf for analysis on FIR bit growth
    unsigned worstCaseBitGrowth = static_cast<unsigned>(ceil(log2(coeffArea)));
    //Construct the fixed point accumulator with enough width to avoid overflow
    unsigned bitsRequired = conf.wlCoeff + conf.wlDelay + worstCaseBitGrowth;
    assert(bitsRequired < 52); //Lattice ECP5 max accumulator width

    m_accum.setFormat(bitsRequired, maxIntWidth + worstCaseBitGrowth);
    log_debug("FIR coefficient format is Q%d.%d, Accumulator format is Q%d.%d (max growth = %d)", maxIntWidth, conf.wlCoeff - maxIntWidth, maxIntWidth + worstCaseBitGrowth, bitsRequired - (maxIntWidth + worstCaseBitGrowth), worstCaseBitGrowth);
}

bool FixedFIR::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT);
    SLFixComplex sample;
    sample.setFormat(data.fc);
    sample = data.fc;

    filter(sample);
    return true;
}

bool FixedFIR::output(filter_io_t &data)
{
    data = m_output;
    return true;
}

void FixedFIR::tick()
{
}

void FixedFIR::filter(SLFixComplex &input)
{
    m_accum = 0;
    m_x.push_front(input);

    for (unsigned int j = 0; (j < m_x.size()); j++) {
        m_accum += (m_x[j] * m_coeffs[j]);
    } //Accumulate

    m_output = m_accum;
}
