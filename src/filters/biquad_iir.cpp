#include <core/logger.hpp>
#include <filters/biquad_iir.hpp>
#include <cassert>

BiquadIIR::BiquadIIR(size_t numStages) :
    FilterChainElement("BiquadIIR"),
    m_stages(numStages, Biquad(18)),
    m_outputReady(false)
{

}

bool BiquadIIR::input(const filter_io_t &data)
{
    m_stages[0].input(data);
    return true;
}

bool BiquadIIR::output(filter_io_t &data)
{
    if (m_outputReady) {
        data = m_sample;
        m_outputReady = false;
        return true;
    }
    return false;
}

void BiquadIIR::tick(void)
{
    bool previousStageDidOutput = false;
    for (size_t i = 0; i < m_stages.size(); i++) {
        if (previousStageDidOutput) {
            m_stages[i].input(m_sample);
        }
        m_stages[i].tick();
        previousStageDidOutput = m_stages[i].output(m_sample);
    }
    m_outputReady = previousStageDidOutput; //did the last stage output?
}

void BiquadIIR::init(const std::vector<Biquad::SOSCoeffs> &coeffs)
{
    assert(coeffs.size() == m_stages.size());
    for (size_t i = 0; i < m_stages.size(); i++) {
        m_stages[i].init(coeffs[i]);
    }
    log_debug("Initialized %d second-order sections for IIR filter", m_stages.size());
}
