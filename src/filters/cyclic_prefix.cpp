#include <filters/cyclic_prefix.hpp>
#include <filters/fft.hpp>
#include <core/logger.hpp>
#include <cassert>

CyclicPrefix::CyclicPrefix(size_t N, size_t cpLen, size_t ticksPerOutput, MCS mcs) :
    FilterChainElement("CyclicPrefix", mcs.getNumCodeWords() * mcs.getCodeWordLength()),
    m_len(cpLen),
    m_symbolLen(N),
    m_inputIdx(0),
    m_outputIdx(0),
    m_ticksPerOutput(ticksPerOutput),
    m_ticksSinceOutput(ticksPerOutput),
    m_outputActive(false),
    m_outputs(N + cpLen)
{
    assert (cpLen < N);
}

bool CyclicPrefix::output(filter_io_t &data)
{
    bool shouldOutputThisCycle = (m_ticksSinceOutput >= m_ticksPerOutput);

    if (shouldOutputThisCycle) {
        if (m_outputActive) {
            m_ticksSinceOutput = 0;
            data.type = IO_TYPE_COMPLEX_FIXPOINT;
            data.fc.setFormat(m_outputs[m_outputIdx]);
            data.fc = m_outputs[m_outputIdx];
            if (++m_outputIdx >= m_outputs.size()) {
                m_outputIdx = 0;
                m_outputActive = false;
            }
        } else {
            data.type = IO_TYPE_COMPLEX_FIXPOINT;
            data.fc.setFormat(FFT_OUTPUT_FORMAT);
            data.fc.set(0.0, 0.0);
        }
    }

    if (shouldOutputThisCycle) {
        m_ticksSinceOutput = 0;
    }
    return shouldOutputThisCycle;
}

void CyclicPrefix::tick()
{

//    assert(m_neverReceivedInput || m_outputActive || !m_fifo.empty());
    if (!m_outputActive && m_fifo.size() >= m_symbolLen) {
        assert((m_outputIdx == 0)); //we should not be in the middle of outputting
        //Fill the symbol
        for (size_t i = 0; i < m_symbolLen; i++) {
            filter_io_t input = m_fifo.front();
            assert(input.type == IO_TYPE_COMPLEX_FIXPOINT);
            SLFixComplex sample = input.fc;
            m_fifo.pop_front();
            m_outputs[i + m_len].setFormat(sample);
            m_outputs[i + m_len] = sample;
        }
        //Prepend the cyclic prefix
        for (size_t i = 0; i < m_len; i ++) {
            m_outputs[i].setFormat(m_outputs[m_symbolLen + i]);
            m_outputs[i] = m_outputs[m_symbolLen + i];
        }
        m_outputActive = true;
    }
    m_ticksSinceOutput++;
}
