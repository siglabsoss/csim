#pragma once

#include <core/filter_chain_element.hpp>
#include <utils/mcs.hpp>
#include <types/circularbuffer.hpp>

class OFDMFrameSync : public FilterChainElement
{
public:

    OFDMFrameSync(size_t cpLen,
                  MCS    mcs);
    bool output(filter_io_t& data) override;
    void tick(void) override;

private:

    ssize_t findPeak();
    void    updateSlidingCalculations();
    enum state_t {
        STATE_FINDING_PEAK = 0,
        STATE_DROP_PREAMBLE,
        STATE_DROP_PREFIX,
        STATE_PASS_SYMBOL
    };
    size_t m_cpLen;
    MCS    m_mcs;
    ComplexDouble m_P;
    double        m_R;
    state_t       m_state;
    double        m_peak;
    CircularBuffer<double> m_timingMetrics;

    // Persistent values used by state machine
    size_t  m_sampleCounter;
    size_t  m_symbolCounter;
    ssize_t m_frameOffset;

    // Output buffer
    filter_io_t m_sample;
    bool m_shouldOutput;
};
