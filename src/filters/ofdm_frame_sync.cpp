#include <filters/ofdm_frame_sync.hpp>

OFDMFrameSync::OFDMFrameSync(size_t cpLen,
                             MCS    mcs) :
    FilterChainElement("FrameSync", mcs.getNumSubCarriers() * 2),
    m_Nfft(mcs.getNumSubCarriers()),
    m_cpLen(cpLen),
    m_mcs(mcs),
    m_P(0.0, 0.0),
    m_R(0.0)
{}

void OFDMFrameSync::tick()
{
    static size_t count = 0;

    // Wait until we have more than a symbol's worth of samples
    if (m_fifo.size() <= m_Nfft) {
        return;
    }
    size_t L = m_Nfft / 2; // length of repeating pilot
                           // in time-domain

    SLFixComplex newest = m_fifo[2 * L].fc;
    SLFixComplex old    = m_fifo[1 * L].fc;
    SLFixComplex oldest = m_fifo[0 * L].fc;

    // TODO fixed point calculations
    m_P = m_P + ((*old * newest) - (*oldest * old)).toComplexDouble();
    m_R = m_R + std::norm(newest.toComplexDouble()) -
          std::norm(old.toComplexDouble());

    if (++count > 2 * L) {
        double timingMetric = std::norm(m_P) / (m_R * m_R);

        // TODO actually use the timing metric to synchronize instead of
        // printing it out
        std::cout << timingMetric << std::endl;
    }

    m_fifo.pop_front();
}

bool OFDMFrameSync::output(filter_io_t& data)
{
    return false;
}
