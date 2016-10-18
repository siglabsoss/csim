#include <filters/zero_pad_interpolator.hpp>
#include <core/logger.hpp>

ZeroPadInterpolator::ZeroPadInterpolator(size_t chunkSize, size_t upSampleFactor) :
    FilterChainElement("FixedZeroPadInterp"),
    m_inputBuffer(chunkSize, FixedComplex32(0.0,0.0)),
    m_outputBuffer(chunkSize*upSampleFactor, FixedComplex32(0.0,0.0)),
    m_shouldOutput(false),
    m_outputIdx(0)
{
    m_inputBuffer.clear();
}

bool ZeroPadInterpolator::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_INT32_COMPLEX);
    if (m_inputBuffer.size() < m_inputBuffer.capacity()) {
        m_inputBuffer.push_back(FixedComplex32(data.toComplexDouble().real(), data.toComplexDouble().imag()));
        return true;
    }
    return false; //overrun of input buffer!
}

bool ZeroPadInterpolator::output(filter_io_t &data)
{
    if (m_shouldOutput == true) {
        //XXX find out why data = m_outputBuffer[m_outputIdx++]; was considered ambiguous
        data.type = IO_TYPE_FIXED_COMPLEX;
        data.fc = m_outputBuffer[m_outputIdx++];
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
            size_t half = m_inputBuffer.size() / 2;
            size_t secondHalfStart = m_outputBuffer.size() - (m_inputBuffer.size() - half);
            for(size_t i = 0; i < m_inputBuffer.size(); i++) {
                if (i < half) {
                    m_outputBuffer[i] = m_inputBuffer[i]; //first half goes on the left side
                } else {
                    m_outputBuffer[secondHalfStart + (i - half)] = m_inputBuffer[i]; //second half on the right, leaving zeros in the middle
                }

            }
            m_inputBuffer.clear();
            m_shouldOutput = true;
        } else { //filled up input buffer before previous output was complete
            log_err("Input buffer full before output was flushed!");
        }
    }
}
