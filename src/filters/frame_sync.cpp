#include <filters/frame_sync.hpp>
#include <cassert>

//There is some variance to the delay, which changes based on the stream of bits flowing through the filters.
//Right now this number obtained by xcorr in MATLAB based on the data dumps from the
//filter_loopback app. This will suffice for now since this block is temporary


//static size_t constexpr FRAME_SYNC_SAMPLE_DELAY = 1420; //determined empirically for 802.16 code
static size_t constexpr FRAME_SYNC_SAMPLE_DELAY = 1240; //determined empirically for 802.11n code

FrameSync::FrameSync(size_t N, size_t cpLen) :
        m_state(STATE_WAIT_FOR_FRAME),
        m_Nfft(N),
        m_cpLen(cpLen),
        m_sampleCount(0),
        m_totalCount(0),
        m_gotInput(false),
        m_sample()
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
            if (m_sampleCount > FRAME_SYNC_SAMPLE_DELAY) {
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
