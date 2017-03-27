#pragma once

#include <core/filter_chain_element.hpp>
#include <utils/mcs.hpp>
#include <types/circularbuffer.hpp>

class OFDMFrameSync : public FilterChainElement
{
public:

    OFDMFrameSync(size_t cpLen,
                  size_t autoCorrLen,
                  MCS    mcs);
    bool   output(filter_io_t& data) override;
    void   tick(void) override;

    double getTimingMetric() const;
    size_t getPeakDetectionCount() const;
    size_t getFrameDetectionCount() const;

private:

    ssize_t findPeak();
    void    updateSlidingCalculations();
    void    initializeSlidingCalculations();
    enum state_t {
        STATE_FINDING_PEAK = 0,
        STATE_DROP_PREAMBLE,
        STATE_DROP_PREFIX,
        STATE_PASS_SYMBOL
    };
    size_t m_cpLen;
    size_t m_autoCorrLen;
    MCS    m_mcs;
    bool   m_didInit;

    // Performance / profiling variables
    size_t m_peakDetectionCount;
    size_t m_frameDetectionCount;

    // Persistent values used by peak finding logic
    ssize_t m_findPeakCounter;
    ssize_t m_lastPeakIdx;
    ComplexDouble m_P;
    double        m_R;
    double        m_peak;
    bool          m_wasAboveThreshold;
    bool          m_didFindPeak;
    CircularBuffer<double> m_timingMetrics;

    // Persistent values used by state machine
    state_t m_state;
    size_t  m_sampleCounter;
    size_t  m_symbolCounter;
    ssize_t m_frameOffset;
    size_t  m_numSamplesToDrop;

    // Output buffer
    filter_io_t m_sample;
    bool m_shouldOutput;
};
