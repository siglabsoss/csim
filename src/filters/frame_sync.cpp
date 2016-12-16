#include <filters/frame_sync.hpp>
#include <cassert>

FrameSync::FrameSync(size_t N, size_t cpLen, size_t sampleDelay) :
        m_state(STATE_WAIT_FOR_FRAME),
        m_Nfft(N),
        m_cpLen(cpLen),
        m_sampleCount(0),
        m_totalCount(0),
        m_gotInput(false),
        m_sample(),
        m_sampleDelay(sampleDelay)
{

}

bool FrameSync::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT);
    m_gotInput = true;
    m_sample = data;
    return true;
}

void FrameSync::tick()
{

}

bool FrameSync::output(filter_io_t &data)
{
    bool didOutputSample = false;
    if (m_gotInput) {
        m_sampleCount++;
        m_totalCount++; //this exists for debugging purposes only
    }
    switch (m_state) {
        case STATE_WAIT_FOR_NONZERO:
            if (m_gotInput) {
                if (std::abs(m_sample.toComplexDouble()) > 0.0) {
                    m_state = STATE_WAIT_FOR_FRAME;
                    m_sampleCount = 1; //we'll reset and count this as our first sample
                }
            }
            break;
        case STATE_WAIT_FOR_FRAME:
            if (m_sampleCount > m_sampleDelay) {
                m_sampleCount = 1;
                m_state = STATE_DROP_PREFIX;
            }
            break;
        case STATE_DROP_PREFIX:
            if (m_sampleCount == m_cpLen) {
                m_state = STATE_PASS_FRAME;
            }
            break;
        case STATE_PASS_FRAME:
            if (m_sampleCount >= m_cpLen + m_Nfft) {
                m_state = STATE_DROP_PREFIX;
                m_sampleCount = 0;
            }
            if (m_gotInput) {
                data = m_sample;
                didOutputSample = true;
            }
            break;
    }

    m_gotInput = false;
    return didOutputSample;
}
