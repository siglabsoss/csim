#include <filters/fixed_fir.hpp>
#include <iostream>

#include <core/logger.hpp>
#include <utils/utils.hpp>

FixedFIR::FixedFIR(std::vector<double> coeffs, Config conf) :
    FilterChainElement("FixedFIR"),
    m_coeffs(coeffs.size()),
    m_x(coeffs.size(), SLFixComplex(conf.wlDelay, conf.iwlDelay, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE)),
    m_output(conf.wlOut, conf.iwlOut, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE),
    m_accum(),
    m_rateAdj(conf.rateChange),
    m_iteration(0),
    m_outputReady(false)
{
    unsigned maxIntWidth = 0;
    double coeffArea = 0.0; // "coefficient area" defined as the sum of the magnitudes, used to determine accumulator width.
    //Find the number of integer bits needed to store the largest coefficient
    for (unsigned int i = 0; i < coeffs.size(); i++) {
        if (coeffs[i] != 0.0) {
            ssize_t intWidth = utils::getShiftAmount(coeffs[i]);
            std::cout << "intWidth = " << intWidth << " for " << coeffs[i] << std::endl;
            coeffArea += fabs(coeffs[i]);
            if (intWidth > maxIntWidth) {
                maxIntWidth = intWidth;
            }
        }
    }
    ++maxIntWidth; //accomodate sign bit
    assert(maxIntWidth <= conf.wlCoeff);

    //Construct the fixed point coefficients with scaling of 2^(wlCoeff - maxIntWidth)
    for (unsigned int i = 0; i < coeffs.size(); i++) {
        m_coeffs[i].setFormat(conf.wlCoeff, maxIntWidth, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
        m_coeffs[i] = coeffs[i];
    }

    //See http://www.digitalsignallabs.com/fir.pdf for analysis on FIR bit growth
    //unsigned worstCaseBitGrowth = static_cast<unsigned>(ceil(log2(coeffArea)));
    //Construct the fixed point accumulator with enough width to avoid overflow
    //unsigned bitsRequired = conf.wlCoeff + conf.wlDelay + worstCaseBitGrowth;
    //assert(bitsRequired <= 52); //Lattice ECP5 max accumulator width
    m_accum.setFormat(52, conf.iwlOut + m_coeffs[0].iwl(), SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    //m_accum.setFormat(bitsRequired, maxIntWidth + worstCaseBitGrowth, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    log_debug("FIR coefficient format is Q%d.%d, Accumulator format is Q%d.%d. Number of taps is %d", m_coeffs[0].iwl(), m_coeffs[0].wl() - m_coeffs[0].iwl(), m_accum.real().iwl(), m_accum.real().wl() - m_accum.real().iwl(), coeffs.size());
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
    if (m_outputReady) {
        m_outputReady = false;
        data = m_output;
        m_iteration = 0;
        return true;
    }
    return false;
}

void FixedFIR::tick()
{
}

void FixedFIR::filter(SLFixComplex &input)
{
    m_accum = 0;
    m_x.push_front(input);

    bool downSample = (m_rateAdj < 0);
    size_t rateAdj = abs(m_rateAdj);

    m_iteration++;

    if (!downSample || m_iteration >= rateAdj) {
        for (unsigned int j = 0; (j < m_x.size()); j++) {
            m_accum += (m_x[j] * m_coeffs[j]);
        } //Accumulate
        m_output = m_accum;
        m_outputReady = true;
    }
}
