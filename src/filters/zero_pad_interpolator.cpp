#include <filters/zero_pad_interpolator.hpp>
#include <core/logger.hpp>

ZeroPadInterpolator::ZeroPadInterpolator(size_t chunkSize) :
    FilterChainElement("FixedZeroPadInterp"),
    m_inputBuffer(chunkSize),
    m_outputBuffer(chunkSize*2),
    m_shouldOutput(false),
    m_outputIdx(0)
{
    m_inputBuffer.clear();
}

bool ZeroPadInterpolator::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_FIXED_COMPLEX_32_NEW);
    if (m_inputBuffer.size() < m_inputBuffer.capacity()) {
        m_inputBuffer.push_back(data.fcn32);
        return true;
    }
    return false; //overrun of input buffer!
}

bool ZeroPadInterpolator::output(filter_io_t &data)
{
    if (m_shouldOutput == true) {
        //XXX find out why data = m_outputBuffer[m_outputIdx++]; was considered ambiguous
        data.type = IO_TYPE_FIXED_COMPLEX_32_NEW;
        data.fcn32 = m_outputBuffer[m_outputIdx++];
    }

    bool didOutput = m_shouldOutput;

    if (m_outputIdx >= m_outputBuffer.size()) {
        m_outputIdx = 0;
        m_shouldOutput = false;
    }

    return didOutput;
}

void ZeroPadInterpolator::tick(void)
{
    if (m_inputBuffer.size() == m_inputBuffer.capacity()) { //we have enough input data
        if (!m_shouldOutput) { //we're not currently outputting
            for(size_t i = 0; i < m_inputBuffer.size(); i++) {
                m_outputBuffer[m_inputBuffer.size()/2 + i] = m_inputBuffer[i];
            }
            m_inputBuffer.clear();
            m_shouldOutput = true;
        } else { //filled up input buffer before previous output was complete
            log_err("Input buffer full before output was flushed!");
        }
    }
}
