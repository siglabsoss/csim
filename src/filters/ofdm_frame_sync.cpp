#include <filters/ofdm_frame_sync.hpp>

// This is a threshold placed on a normalized value, thus it does not depend
// on the power level of the input signal.
static constexpr double TIMING_METRIC_MIN_PEAK = 0.5;

// This threshold depends on the power level of the input signal and will need
// to change if some kind of AGC block comes earlier in the chain.
static constexpr double POWER_EST_MIN_THRESHOLD = 0.0;

OFDMFrameSync::OFDMFrameSync(size_t cpLen,
                             size_t numTrainingSym,
                             MCS    mcs) :
    FilterChainElement("FRAME_SYNC", 4 * (numTrainingSym) * (cpLen + mcs.getNumSubCarriers())),
    m_cpLen(cpLen),
    m_numTrainingSym(numTrainingSym),
    m_peakFindingWindowWidth(512 + 100),
    m_timingMetricMaxHistory(2 * m_peakFindingWindowWidth),
    m_mcs(mcs),
    m_L((numTrainingSym / 2) * (cpLen + mcs.getNumSubCarriers())),
    m_didInit(false),
    m_peakDetectionCount(0),
    m_frameDetectionCount(0),
    m_findPeakCounter(-1),
    m_lastPeakIdx(0),
    m_P(0.0, 0.0),
    m_R(0.0),
    m_peak(0.0),
    m_wasAboveThreshold(false),
    m_didFindPeak(false),
    m_timingMetrics(m_timingMetricMaxHistory),
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
    for (size_t i = 0; i < m_L; ++i) {
        size_t end         = m_fifo.size() - 1;
        SLFixComplex first = m_fifo[end - (2 * m_L) + i].fc;
        SLFixComplex last  = m_fifo[end - (1 * m_L) + i].fc;
        m_P = m_P + (*first * last).toComplexDouble();
        m_R = m_R + std::norm(last.toComplexDouble());
    }
}

void OFDMFrameSync::updateSlidingCalculations()
{
    size_t end = m_fifo.size() - 1;

    SLFixComplex oldest = m_fifo[end - (2 * m_L)].fc;
    SLFixComplex old    = m_fifo[end - (1 * m_L)].fc;
    SLFixComplex newest = m_fifo[end - (0 * m_L)].fc;

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
    if (powerEst == 0.0) {
        timingMetric = 0.0;
    } else {
        timingMetric = std::norm(m_P) / powerEst;
    }
    m_timingMetrics.push_back(timingMetric);
}

ssize_t OFDMFrameSync::findPeak()
{
    // most recent timing metric value
    double  timingMetric = m_timingMetrics[m_timingMetrics.size() - 1];
    ssize_t retval       = -1;

    // static size_t tmCount = 0;

    // std::cout << timingMetric << "," << std::norm(m_P) <<
    // "," << m_R * m_R <<
    // std::endl;

    // static size_t tmCount = 0;
    //
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
                m_findPeakCounter = m_peakFindingWindowWidth;
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
            // std::cout << "***New peak at " << timingMetric << std::endl;
            m_peak        = timingMetric;
            m_lastPeakIdx = m_timingMetrics.size() - 1;
        } else {
            m_lastPeakIdx--;
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
        ssize_t peakIdx    = m_lastPeakIdx;
        ssize_t leftIdx    = peakIdx;
        ssize_t rightIdx   = peakIdx;
        bool    foundLeft  = false;
        bool    foundRight = false;

        double tm;

        // This can't happen if the timer is shorter than the timing metric
        // history buffer, but checking anyway
        assert(peakIdx >= 0);

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
        m_peakDetectionCount++;

        if (foundLeft && foundRight) {
            ssize_t frameStartDelay = leftIdx + (rightIdx - leftIdx) / 2;

            std::cout << "***Frame start is set to " <<
            m_timingMetricMaxHistory - frameStartDelay <<
            " samples ago. Peak timing metric was " << m_peak <<
            std::endl;
            std::cout << "leftIdx = " << leftIdx << " rightIdx = " << rightIdx <<
            " peakIdx = " << peakIdx << std::endl;

            retval = frameStartDelay;
        } else {
            std::cout <<
            "***Did not find 90% drop off point on both sides of peak using peak at "
                      <<
            m_timingMetricMaxHistory - peakIdx << " samples ago" << std::endl;
            retval = peakIdx;
        }

        m_peak = 0.0;
        return retval;
    }

    return retval;
}

void OFDMFrameSync::tick()
{
    if (m_fifo.size() > m_L * 2) {
        if (m_didInit == false) {
            std::cout << "Frame Sync FIFO has " << m_L * 2 << " samples. Initializing" << std::endl;
            initializeSlidingCalculations();
            m_didInit = true;
        } else {
            if (m_gotInput == true) {
                updateSlidingCalculations();
                m_gotInput = false;
            }
        }
    }

    if (m_fifo.size() < (m_L * 2) + m_timingMetricMaxHistory) {
        return;
    }

    static const size_t TOTAL_PREAMBLE_LENGTH = m_numTrainingSym *
                                                (m_mcs.getNumSubCarriers() +
                                                 m_cpLen);

    ssize_t frameOffset = findPeak();

    if (frameOffset < 0) {
        m_frameOffset--;
    } else {
        if (m_didFindPeak == false) {
            std::cout << "***Found frame at offset " << frameOffset <<
            " current state = " << m_state << std::endl;
            m_frameOffset = frameOffset;
            m_didFindPeak = true;
        } else {
            std::cout <<
            "***Using previously found frame at current offset of " <<
            m_frameOffset << std::endl;
        }
    }

    switch (m_state) {
    case STATE_FINDING_PEAK:
    {
        if (m_didFindPeak) {
            m_state            = STATE_DROP_PREAMBLE;
            m_numSamplesToDrop = TOTAL_PREAMBLE_LENGTH + m_frameOffset;

            std::cout << "***Frame is at offset " << m_frameOffset <<
            " dropping the next " << m_numSamplesToDrop << " samples" <<
            std::endl;
        }
        break;
    }

    case STATE_DROP_PREAMBLE:

        if ((m_sampleCounter < m_numSamplesToDrop)) {
            m_sampleCounter++;
        }

        if (m_sampleCounter >= m_numSamplesToDrop) {
            std::cout << "***Dropped " << m_sampleCounter <<
            " samples, passing frame" << std::endl;
            m_state         = STATE_PASS_SYMBOL;
            m_sampleCounter = 0;
            m_didFindPeak   = false; // allow us to find peaks again
        }
        break;

    case STATE_DROP_PREFIX:
    {
        if (m_sampleCounter < m_cpLen) {
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
            m_sampleCounter++;
        }

        if (m_sampleCounter >= numSamplesPerSymbol) {
            if (++m_symbolCounter >= numSymbolsPerFrame) {
                std::cout << "***Passed " << m_symbolCounter <<
                " OFDM symbols, which makes up a full frame" << std::endl;
                m_state         = STATE_FINDING_PEAK;
                m_symbolCounter = 0;
                m_frameDetectionCount++;
            } else {
                // std::cout << "***Passed OFDM symbol of " << m_sampleCounter
                // <<
                // " samples" << std::endl;
                m_state = STATE_DROP_PREFIX;
            }
            m_sampleCounter = 0;
        }
        break;
    }
    }

    m_fifo.pop_front();
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

double OFDMFrameSync::getTimingMetric() const
{
    return m_timingMetrics.back();
}

size_t OFDMFrameSync::getPeakDetectionCount() const
{
    return m_peakDetectionCount;
}

size_t OFDMFrameSync::getFrameDetectionCount() const
{
    return m_frameDetectionCount;
}
