#include <filters/ofdm_frame_sync.hpp>

static constexpr size_t MIN_PLATEAU_WIDTH      = 2;
static constexpr double TIMING_METRIC_MIN_PEAK = 0.1;
static constexpr size_t TOTAL_PREAMBLE_LENGTH  = 1024 * 2 + 100;

OFDMFrameSync::OFDMFrameSync(size_t cpLen,
                             MCS    mcs) :
    FilterChainElement("FrameSync", mcs.getNumSubCarriers() * 2),
    m_cpLen(cpLen),
    m_mcs(mcs),
    m_P(0.0, 0.0),
    m_R(0.0),
    m_state(STATE_FINDING_PEAK),
    m_peak(0.0),
    m_timingMetrics(cpLen * 2),
    m_sampleCounter(0),
    m_symbolCounter(0),
    m_frameOffset(0),
    m_sample(),
    m_shouldOutput(false)
{}

void OFDMFrameSync::updateSlidingCalculations()
{
    if ((m_fifo.size() <= m_mcs.getNumSubCarriers())) {
        return;
    }
    size_t L = m_mcs.getNumSubCarriers() / 2; // length of repeating pilot
    // in time-domain

    SLFixComplex newest = m_fifo[2 * L].fc;
    SLFixComplex old    = m_fifo[1 * L].fc;
    SLFixComplex oldest = m_fifo[0 * L].fc;

    // TODO fixed point calculations
    // By adding the newest element of the summation and subtracting out the
    // oldest we're, effectively 'sliding' the summation along a series of
    // samples in an iterative fashion
    m_P = m_P + ((*old * newest) - (*oldest * old)).toComplexDouble();
    m_R = m_R + std::norm(newest.toComplexDouble()) -
          std::norm(old.toComplexDouble());

    double timingMetric = 0.0;

    if (m_R != 0.0) {
        timingMetric = std::norm(m_P) / (m_R * m_R);
    }
    m_timingMetrics.push_back(timingMetric);
}

ssize_t OFDMFrameSync::findPeak()
{
    // XXX didInit flag
    static size_t initCount = 0;

    if (++initCount <= m_mcs.getNumSubCarriers()) {
        return -1;
    }

    size_t L = m_mcs.getNumSubCarriers() / 2; // length of repeating
                                              // pilot

    // most recent timing metric value
    double  timingMetric = m_timingMetrics[m_timingMetrics.size() - 1];
    ssize_t retval       = -1;

    if (timingMetric > m_peak) {
        m_peak = timingMetric;
    }

    if ((m_peak > TIMING_METRIC_MIN_PEAK) &&
        (timingMetric < (0.9 * m_peak))) {
        // Now that we've dropped below 90% of the peak, we can
        // review the timing metric signal and find the average
        // in between now and the point in which we exceeded 90% of
        // the peak
        size_t sampleCount = 0, idx = 0;
        double tm;

        do {
            sampleCount++;
            idx = m_timingMetrics.size() - 1 - sampleCount;
            tm  = m_timingMetrics[idx];

            // std::cout << "m_timingMetrics[" << idx << "] = " << tm <<
            // std::endl;
            // std::cout << "threshold is = " << 0.9 * m_peak <<
            // std::endl;
        } while ((tm >= (0.9 * m_peak)) && (idx > 0));

        // std::cout << "loop exit " << tm << " < " <<
        // (0.9 * m_peak) << " || " << idx << " == 0" << std::endl;

        if ((tm >= (0.9 * m_peak)) ||
            (sampleCount < MIN_PLATEAU_WIDTH)) {
            // We didn't quite detect a real plateau. Keep trying.
        } else {
            // At this point we know how many samples ago we're going to
            // consider to be the start of the frame, so we can move on.
            size_t frameStartDelay = (sampleCount / 2);
            std::cout << "Frame start is set to " << frameStartDelay <<
            " samples ago. Peak timing metric was " << m_peak <<
            std::endl;

            // for (size_t numSamplesToToss = (2 * L) - frameStartDelay;
            //      numSamplesToToss > 0; --numSamplesToToss) {
            //     m_fifo.pop_front();
            // }
            // std::cout << "Fast forwarding by dropping " <<
            // (2 * L) - frameStartDelay << " samples to reach start of frame."
            //           << std::endl;
            retval = (2 * L) - frameStartDelay;
        }
        m_peak = 0.0;
        return retval;
    }
    return retval;
}

void OFDMFrameSync::tick()
{
    // Wait until we have more than a symbol's worth of samples
    // if (m_gotInput) {
    //     m_sampleCount++;
    //     m_totalCount++; // this exists for debugging purposes only
    // }
    if ((m_fifo.size() <= m_mcs.getNumSubCarriers())) {
        return;
    }

    updateSlidingCalculations();
    ssize_t frameOffset = findPeak();

    if (frameOffset < 0) {
        m_frameOffset--;
    } else {
        m_frameOffset = frameOffset;
    }

    // std::cout << "frame offset = " << m_frameOffset << std::endl;

    switch (m_state) {
    case STATE_FINDING_PEAK:
    {
        m_fifo.pop_front();

        if (m_frameOffset >= 0) {
            m_state = STATE_DROP_PREAMBLE;
            std::cout << "Found frame at offset " << m_frameOffset << std::endl;
        }
        break;
    }

    case STATE_DROP_PREAMBLE:

        if ((m_sampleCounter < TOTAL_PREAMBLE_LENGTH + m_frameOffset) &&
            (m_fifo.size() > 0)) {
            m_fifo.pop_front();
            m_sampleCounter++;
        }

        if (m_sampleCounter >= TOTAL_PREAMBLE_LENGTH + m_frameOffset) {
            std::cout << "Dropped " << m_sampleCounter <<
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
                std::cout << "Passed " << m_symbolCounter <<
                " OFDM symbols, which makes up a full frame" << std::endl;
                m_state         = STATE_FINDING_PEAK;
                m_symbolCounter = 0;
            } else {
                std::cout << "Passed OFDM symbol of " << m_sampleCounter <<
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
