#include <filters/ofdm_frame_sync.hpp>

static constexpr size_t MIN_PLATEAU_WIDTH         = 50;
static constexpr size_t MAX_TIMING_METRIC_HISTORY = 200;

// This is a threshold placed on a normalized value, thus it does not depend
// on the power level of the input signal.
static constexpr double TIMING_METRIC_MIN_PEAK = 0.1;

// This threshold depends on the power level of the input signal and will need
// to change if some kind of AGC block comes earlier in the chain.
static constexpr double POWER_EST_MIN_THRESHOLD = 0.001;

OFDMFrameSync::OFDMFrameSync(size_t cpLen,
                             MCS    mcs) :
    FilterChainElement("FrameSync", mcs.getNumSubCarriers() * 2 * 10),
    m_cpLen(cpLen),
    m_mcs(mcs),
    m_P(0.0, 0.0),
    m_R(0.0),
    m_state(STATE_FINDING_PEAK),
    m_peak(0.0),
    m_timingMetrics(MAX_TIMING_METRIC_HISTORY),
    m_sampleCounter(0),
    m_symbolCounter(0),
    m_frameOffset(-1),
    m_numSamplesToDrop(0),
    m_sample(),
    m_shouldOutput(false)
{}

void OFDMFrameSync::updateSlidingCalculations()
{
    if (m_fifo.size() <= m_mcs.getNumSubCarriers() + MAX_TIMING_METRIC_HISTORY) {
        return;
    }
    size_t L = m_mcs.getNumSubCarriers() / 2; // length of repeating pilot
    // in time-domain

    SLFixComplex newest = m_fifo[MAX_TIMING_METRIC_HISTORY + 2 * L].fc;
    SLFixComplex old    = m_fifo[MAX_TIMING_METRIC_HISTORY + 1 * L].fc;
    SLFixComplex oldest = m_fifo[MAX_TIMING_METRIC_HISTORY + 0 * L].fc;

    // TODO fixed point calculations and avoid the division
    // By adding the newest element of the summation and subtracting out the
    // oldest, we're effectively 'sliding' the summation along a series of
    // samples in an iterative fashion
    m_P = m_P + ((*old * newest) - (*oldest * old)).toComplexDouble();
    m_R = m_R + std::norm(newest.toComplexDouble()) -
          std::norm(old.toComplexDouble());

    double timingMetric;
    double powerEst = (m_R * m_R);

    if (powerEst < POWER_EST_MIN_THRESHOLD) {
        timingMetric = 0.0;
    } else {
        timingMetric = std::norm(m_P) / powerEst;
    }
    m_timingMetrics.push_back(timingMetric);
}

ssize_t OFDMFrameSync::findPeak()
{
    updateSlidingCalculations();

    // XXX didInit flag
    static size_t initCount = 0;

    if (++initCount <= m_mcs.getNumSubCarriers()) {
        return -1;
    }

    // most recent timing metric value
    double  timingMetric = m_timingMetrics[m_timingMetrics.size() - 1];
    ssize_t retval       = -1;

    // std::cout << timingMetric << std::endl;

    if (timingMetric > m_peak) {
        m_peak = timingMetric;
    }

    if ((m_peak > TIMING_METRIC_MIN_PEAK) &&
        (timingMetric < (0.9 * m_peak))) {
        // Now that we've dropped below 90% of the peak, we can
        // review the timing metric signal and find the average
        // in between now and the point in which we exceeded 90% of
        // the peak
        size_t idx = m_timingMetrics.size() - 2;
        double tm;

        do {
            tm = m_timingMetrics[idx--];

            // std::cout << "m_timingMetrics[" << idx << "] = " << tm <<
            // std::endl;
            // std::cout << "threshold is = " << 0.9 * m_peak <<
            // std::endl;
        } while ((tm >= (0.9 * m_peak)) && (idx > 0));

        // std::cout << "loop exit " << tm << " < " <<
        // (0.9 * m_peak) << " || " << idx << " == 0" << std::endl;

        if ((tm >= (0.9 * m_peak)) ||
            (MAX_TIMING_METRIC_HISTORY - idx < MIN_PLATEAU_WIDTH)) {
            // This 'plateau' wasn't quite wide enough to count. Keep looking.
        } else {
            // At this point we can say we found the start of the frame. We
            // will go ahead and calculate it as the midpoint in between the
            // two 90% crossings
            size_t frameStartDelay = ((MAX_TIMING_METRIC_HISTORY - idx) / 2);

            std::cout << "***Frame start is set to " << frameStartDelay <<
            " samples ago. Peak timing metric was " << m_peak <<
            std::endl;

            retval = MAX_TIMING_METRIC_HISTORY - frameStartDelay;
        }
        m_peak = 0.0;
        return retval;
    }
    return retval;
}

void OFDMFrameSync::tick()
{
    // Wait until we have more than a symbol's worth of samples
    if (m_fifo.size() <= m_mcs.getNumSubCarriers() + MAX_TIMING_METRIC_HISTORY) {
        return;
    }

    static const size_t TOTAL_PREAMBLE_LENGTH = m_mcs.getNumSubCarriers() * 2 +
                                                m_cpLen * 2;
    ssize_t frameOffset = findPeak();

    if (frameOffset < 0) {
        m_frameOffset--;
    } else {
        std::cout << "***Found frame at offset " << frameOffset <<
        std::endl;

        if (m_frameOffset < 0) {
            // XXX this hardcoded offset shift is here temporarily
            m_frameOffset = frameOffset + 63;
        } else {
            std::cout << "***Using previously found frame offset of " <<
            m_frameOffset << std::endl;
        }
    }

    switch (m_state) {
    case STATE_FINDING_PEAK:
    {
        m_fifo.pop_front();

        if (m_frameOffset >= 0) {
            m_state            = STATE_DROP_PREAMBLE;
            m_numSamplesToDrop = TOTAL_PREAMBLE_LENGTH + m_frameOffset;

            std::cout << "***Frame is at offset " << m_frameOffset <<
            " dropping the next " << m_numSamplesToDrop << " samples" <<
            std::endl;
        }
        break;
    }

    case STATE_DROP_PREAMBLE:

        if ((m_sampleCounter < m_numSamplesToDrop) &&
            (m_fifo.size() > 0)) {
            m_fifo.pop_front();
            m_sampleCounter++;
        }

        if (m_sampleCounter >= m_numSamplesToDrop) {
            std::cout << "***Dropped " << m_sampleCounter <<
            " samples, passing frame" << std::endl;
            m_state         = STATE_PASS_SYMBOL;
            m_sampleCounter = 0;
        }
        break;

    case STATE_DROP_PREFIX:
    {
        if (m_sampleCounter < m_cpLen) {
            m_fifo.pop_front();
            m_sampleCounter++;
        }

        if (m_sampleCounter >= m_cpLen) {
            m_state         = STATE_PASS_SYMBOL;
            m_sampleCounter = 0;
        }
        break;
    }

    case STATE_PASS_SYMBOL:
    {
        size_t numSamplesPerSymbol = m_mcs.getNumSubCarriers();
        size_t numSymbolsPerFrame  = m_mcs.getNumOFDMSymbols();

        if (m_sampleCounter < numSamplesPerSymbol) {
            m_shouldOutput = true;
            m_sample       = m_fifo.front();
            m_fifo.pop_front();
            m_sampleCounter++;
        }

        if (m_sampleCounter >= numSamplesPerSymbol) {
            if (++m_symbolCounter >= numSymbolsPerFrame) {
                std::cout << "***Passed " << m_symbolCounter <<
                " OFDM symbols, which makes up a full frame" << std::endl;
                m_state         = STATE_FINDING_PEAK;
                m_symbolCounter = 0;
            } else {
                std::cout << "***Passed OFDM symbol of " << m_sampleCounter <<
                " samples" << std::endl;
                m_state = STATE_DROP_PREFIX;
            }
            m_sampleCounter = 0;
        }
        break;
    }
    }
}

bool OFDMFrameSync::output(filter_io_t& data)
{
    if (m_shouldOutput == true) {
        data           = m_sample;
        m_shouldOutput = false;
        return true;
    }
    return false;
}
