#include <filters/frame_sync.hpp>
#include <cassert>

static size_t constexpr INTRA_FRAME_DELAY = 100;
static size_t constexpr DDC_DUC_PHASE_DELAY = 40;

FrameSync::FrameSync(size_t N, size_t cpLen) :
        m_state(STATE_WAIT_FOR_NONZERO),
        m_Nfft(N),
        m_cpLen(cpLen),
        m_sampleCount(0),
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
    }
    switch (m_state) {
        case STATE_WAIT_FOR_NONZERO:
            if (m_gotInput) {
                if (std::abs(m_sample.toComplexDouble()) > 0.0) { //XXX validate this threshold
                    m_state = STATE_WAIT_FOR_FRAME;
                    m_sampleCount = 1; //we'll reset and count this as our first sample
                }
            }
            break;
        case STATE_WAIT_FOR_FRAME:
            if (m_sampleCount > DDC_DUC_PHASE_DELAY) {
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
                m_state = STATE_POST_FRAME_DELAY;
            }
            if (m_gotInput) {
                data = m_sample;
                didOutputSample = true;
            }
            break;
        case STATE_POST_FRAME_DELAY:
            if (m_sampleCount >= m_cpLen + m_Nfft + INTRA_FRAME_DELAY) {
                  m_state = STATE_WAIT_FOR_NONZERO;
            }
    }

    m_gotInput = false;
    return didOutputSample;
}
