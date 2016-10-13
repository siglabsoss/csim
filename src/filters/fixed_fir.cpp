#include <filters/fixed_fir.hpp>
#include <iostream>

#include <core/logger.hpp>
#include <utils/utils.hpp>

FixedFIR::FixedFIR(const std::vector<double> &coeffs, const Config &conf) :
    FilterChainElement("FixedFIR"),
    m_coeffs(),
    m_delayLine(),
    m_output(conf.wlOut, conf.iwlOut, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE),
    m_accum(),
    m_rateAdj(conf.rateChange),
    m_outputReady(false),
    m_lastInput(),
    m_newInput(false),
    m_ticksSinceLastInput(0),
    m_filterIteration(0)
{
    unsigned maxIntWidth = 0;
    double coeffArea = 0.0; // "coefficient area" defined as the sum of the magnitudes, used to determine accumulator width.
    //Find the number of integer bits needed to store the largest coefficient
    for (unsigned int i = 0; i < coeffs.size(); i++) {
        if (coeffs[i] != 0.0) {
            ssize_t intWidth = utils::getShiftAmount(coeffs[i]);
            coeffArea += fabs(coeffs[i]);
            if (intWidth > maxIntWidth) {
                maxIntWidth = intWidth;
            }
        }
    }
    ++maxIntWidth; //accomodate sign bit
    assert(maxIntWidth <= conf.wlCoeff);

    if (m_rateAdj >= -1 && m_rateAdj <= 1) {
        //-1, 0, and 1 all mean "no rate change" use 1 as the canonical value to make calculations easier
        m_rateAdj = 1;
    }

    if (m_rateAdj > 1) {
        //extend coefficients up to the next multiple
        size_t coeffExtention = (m_rateAdj - coeffs.size() % m_rateAdj) % m_rateAdj;
        m_coeffs.resize(coeffs.size() +  coeffExtention);
        log_debug("Interpolation by a factor of %d. Zero padding with %d additional coefficients for polyphase operations.", m_rateAdj, coeffExtention);
    } else {
        m_coeffs.resize(coeffs.size());
    }
    //Construct the fixed point coefficients with scaling of 2^(wlCoeff - maxIntWidth)
    for (unsigned int i = 0; i < coeffs.size(); i++) {
        m_coeffs[i].setFormat(conf.wlCoeff, maxIntWidth, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
        m_coeffs[i] = coeffs[i];
    }

    //Padding extra zero coefficients to simplify polyphase operations
    for (unsigned int i = coeffs.size(); i < m_coeffs.size(); i++) {
        m_coeffs[i].setFormat(conf.wlCoeff, maxIntWidth, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
        m_coeffs[i] = 0.0;
    }

    size_t delaySize = m_coeffs.size();
    //If we are upsampling, our delay line size is a fraction of the coefficient size
    if (m_rateAdj > 1) {
        delaySize /= m_rateAdj;
    }
    m_delayLine = CircularBuffer<SLFixComplex>(delaySize, SLFixComplex(conf.wlDelay, conf.iwlDelay, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));

    if (m_rateAdj < -1) {
        log_debug("Decimation filter downsampling by a factor of %d.", abs(m_rateAdj));
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
    m_lastInput.setFormat(data.fc);
    m_lastInput = data.fc;
    m_newInput = true;

    return true;
}

bool FixedFIR::output(filter_io_t &data)
{
    if (m_outputReady) {
        m_outputReady = false;
        data = m_output;
        return true;
    }
    return false;
}

void FixedFIR::tick()
{
    bool receivedFirstSample = m_lastInput.isFormatSet();
    if (!receivedFirstSample) {
        return;
    }
    if (m_newInput) {
        m_ticksSinceLastInput = 0;
    }

    //If we are downsampling or not rate adjusting, then we should only invoke
    //the filter when we get new input. In the case of upsampling, we need to
    //invoke the filter multiple times per new input
    bool shouldFilter = ((m_rateAdj <= 1) && m_newInput) || (m_rateAdj > 1);

    if (m_rateAdj == 0 || m_rateAdj == 1) {
        assert(m_ticksSinceLastInput == 0);
    } else if (m_rateAdj > 1) {
        assert(m_ticksSinceLastInput < static_cast<size_t>(m_rateAdj));
    }

    if (shouldFilter) {
        filter(m_lastInput, m_ticksSinceLastInput);
    }
    m_ticksSinceLastInput++;
    m_newInput = false;
}

void FixedFIR::filter(SLFixComplex &input, size_t polyPhaseOffset)
{
    m_accum = 0;

    bool noRateAdjustment = (m_rateAdj >= -1 && m_rateAdj <= 1);
    bool downSampling = (m_rateAdj < -1);
    bool newSample = (polyPhaseOffset == 0);
    bool addSampleToDelayLine = noRateAdjustment || newSample;

    if (addSampleToDelayLine) {
        m_delayLine.push_front(input);
    }

    size_t decimationFactor = 0;
    if (downSampling) {
        ++m_filterIteration;
        decimationFactor = abs(m_rateAdj);
    }

    size_t polyPhaseIncrement = 1;
    if (m_rateAdj > 1) {
        polyPhaseIncrement = m_rateAdj;
    }
    if (!downSampling || m_filterIteration >= decimationFactor) {
        unsigned int delayIdx = 0;
        for (unsigned int coeffIdx = polyPhaseOffset; (coeffIdx < m_coeffs.size()); coeffIdx += polyPhaseIncrement) {
            m_accum += (m_delayLine[delayIdx++] * m_coeffs[coeffIdx]);
        } //Accumulate
        m_output = m_accum;
        m_outputReady = true;
        m_filterIteration = 0;
    }
}
