#include <filters/ofdm_frame_sync.hpp>

static constexpr size_t MIN_PLATEAU_WIDTH         = 2;
static constexpr size_t PEAK_FINDING_WINDOW_WIDTH = 512 + 100;

// Timing metric history must be larger than the peak-finding window width
static constexpr size_t MAX_TIMING_METRIC_HISTORY = PEAK_FINDING_WINDOW_WIDTH *
                                                    2;

// This is a threshold placed on a normalized value, thus it does not depend
// on the power level of the input signal.
static constexpr double TIMING_METRIC_MIN_PEAK = 0.8;

// This threshold depends on the power level of the input signal and will need
// to change if some kind of AGC block comes earlier in the chain.
static constexpr double POWER_EST_MIN_THRESHOLD = 0.0002;

OFDMFrameSync::OFDMFrameSync(size_t cpLen,
                             MCS    mcs) :
    FilterChainElement("FrameSync", MAX_TIMING_METRIC_HISTORY * 10),
    m_cpLen(cpLen),
    m_mcs(mcs),
    m_didInit(false),
    m_findPeakCounter(-1),
    m_lastPeakFoundDelay(0),
    m_P(0.0, 0.0),
    m_R(0.0),
    m_peak(0.0),
    m_wasAboveThreshold(false),
    m_timingMetrics(MAX_TIMING_METRIC_HISTORY),
    m_state(STATE_FINDING_PEAK),
    m_sampleCounter(0),
    m_symbolCounter(0),
    m_frameOffset(-1),
    m_numSamplesToDrop(0),
    m_sample(),
    m_shouldOutput(false)
{}

void OFDMFrameSync::initializeSlidingCalculations()
{
    const size_t L = m_mcs.getNumSubCarriers() / 2; // length of repeating pilot

    for (size_t i = 0; i < L; ++i) {
        SLFixComplex first = m_fifo[MAX_TIMING_METRIC_HISTORY + i + 0].fc;
        SLFixComplex last  = m_fifo[MAX_TIMING_METRIC_HISTORY + i + L].fc;
        m_P = m_P + (*first * last).toComplexDouble();
        m_R = m_R + std::norm(last.toComplexDouble());
    }
}

void OFDMFrameSync::updateSlidingCalculations()
{
    // ssize_t end = m_fifo.size() - 1;
    const size_t L = m_mcs.getNumSubCarriers() / 2; // length of repeating pilot

    SLFixComplex newest = m_fifo[MAX_TIMING_METRIC_HISTORY - 1 + 2 * L].fc;
    SLFixComplex old    = m_fifo[MAX_TIMING_METRIC_HISTORY - 1 + 1 * L].fc;
    SLFixComplex oldest = m_fifo[MAX_TIMING_METRIC_HISTORY - 1 + 0 * L].fc;

    // TODO fixed point calculations and avoid the division
    // By adding the newest element of the summation and subtracting out the
    // oldest, we're effectively 'sliding' the summation along a series of
    // samples in an iterative fashion
    m_P = m_P + ((*old * newest) - (*oldest * old)).toComplexDouble();
    m_R = m_R + std::norm(newest.toComplexDouble()) -
          std::norm(old.toComplexDouble());

    double timingMetric;
    double powerEst = (m_R * m_R);

    // std::cout << powerEst << std::endl;

    // if (powerEst <= POWER_EST_MIN_THRESHOLD) {
    //     timingMetric = 0.0;
    // } else {
    //     timingMetric = std::norm(m_P) / powerEst;
    // }
    timingMetric = std::norm(m_P) / powerEst;
    m_timingMetrics.push_back(timingMetric);
}

ssize_t OFDMFrameSync::findPeak()
{
    updateSlidingCalculations();

    // most recent timing metric value
    double  timingMetric = m_timingMetrics[m_timingMetrics.size() - 1];
    ssize_t retval       = -1;

    // std::cout << timingMetric << "," << std::norm(m_P) << "," << m_R * m_R <<
    // std::endl;

    // static size_t tmCount = 0;
    // std::cout << tmCount++ << " " << timingMetric << std::endl;

    // std::cout << timingMetric << std::endl;

    double powerEst                    = (m_R * m_R);
    bool   peakFinderThresholdExceeded =
        (timingMetric > TIMING_METRIC_MIN_PEAK) &&
        (powerEst > POWER_EST_MIN_THRESHOLD);
    bool timerExpired = (m_findPeakCounter <= 0);

    // Start the timer if above threshold and previous timer is not running,
    // but don't restart the counter unless we actually dipped below the
    // threshold after the previous timer expired
    if (peakFinderThresholdExceeded) {
        if (timerExpired) {
            if (!m_wasAboveThreshold) {
                m_wasAboveThreshold = true;
                std::cout << "***Restarting peak detection timer " << std::endl;
                m_findPeakCounter = PEAK_FINDING_WINDOW_WIDTH;
                m_peak            = 0.0;
            }
        }
    } else { // If we're below the threshold
        // ...and the timer expired, reset this flag so that we can restart the
        // timer after we've exceeded the threshold again.
        if (m_findPeakCounter < 0) {
            m_wasAboveThreshold = false;
        }
    }

    // While timer is running, scan for peak
    if (m_findPeakCounter-- > 0) {
        if (timingMetric > m_peak) {
            std::cout << "***New peak at " << timingMetric << std::endl;
            m_peak               = timingMetric;
            m_lastPeakFoundDelay = 0;
        } else {
            m_lastPeakFoundDelay++;
        }
    }

    // Peak finding timing just expired and we have a peak detected.
    if ((m_findPeakCounter <= 0) && (m_peak > TIMING_METRIC_MIN_PEAK)) {
        // Now that we've dropped below 90% of the peak, we can
        // review the timing metric signal and find the average
        // in between now and the point in which we exceeded 90% of
        // the peak
        ssize_t end        = m_timingMetrics.size() - 1;
        ssize_t start      = 0;
        ssize_t peakIdx    = end - m_lastPeakFoundDelay;
        ssize_t leftIdx    = peakIdx;
        ssize_t rightIdx   = peakIdx;
        bool    foundLeft  = false;
        bool    foundRight = false;

        double tm;

        // Find 90% crossing to the left of the peak
        do {
            tm = m_timingMetrics[leftIdx--];
        } while ((tm >= (0.9 * m_peak)) && (leftIdx >= start));

        foundLeft = (leftIdx >= start);

        // Find 90% crossing to the right of the peak
        do {
            tm = m_timingMetrics[rightIdx++];
        } while ((tm >= (0.9 * m_peak)) && (rightIdx <= end));

        foundRight = (rightIdx <= end);

        // std::cout << "loop exit " << tm << " < " <<
        // (0.9 * m_peak) << " || " << idx << " == 0" << std::endl;

        // assert(foundLeft && foundRight);

        if (foundLeft && foundRight) {
            ssize_t frameStartDelay = leftIdx + (rightIdx - leftIdx) / 2;

            std::cout << "***Frame start is set to " <<
            MAX_TIMING_METRIC_HISTORY - frameStartDelay <<
            " samples ago. Peak timing metric was " << m_peak <<
            std::endl;
            std::cout << "leftIdx = " << leftIdx << " rightIdx = " << rightIdx <<
            " peakIdx = " << peakIdx << std::endl;

            retval = frameStartDelay;
        } else {
            std::cout <<
            "***Did not find 90% drop off point on both sides of peak using peak at "
                      <<
            MAX_TIMING_METRIC_HISTORY - peakIdx << " samples ago" << std::endl;
            retval = peakIdx;
        }

        m_peak = 0.0;
        return retval;
    }

    return retval;
}

void OFDMFrameSync::tick()
{
    // Wait until we have more than a symbol's worth of samples
    if (m_fifo.size() < m_mcs.getNumSubCarriers() + MAX_TIMING_METRIC_HISTORY) {
        return;
    }

    if (m_didInit == false) {
        initializeSlidingCalculations();
        m_didInit = true;
    }

    static const size_t TOTAL_PREAMBLE_LENGTH = m_mcs.getNumSubCarriers() * 2 +
                                                m_cpLen * 2;
    ssize_t frameOffset = findPeak();

    if (frameOffset < 0) {
        m_frameOffset--;
    } else {
        std::cout << "***Found frame at offset " << frameOffset <<
        " current state = " << m_state << std::endl;
        m_frameOffset = frameOffset;

        // if (m_frameOffset < 0) {
        //     m_frameOffset = frameOffset;
        // } else {
        //     std::cout << "***Using previously found frame offset of " <<
        //     m_frameOffset << std::endl;
        // }
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
