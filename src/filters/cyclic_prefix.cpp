#include <filters/cyclic_prefix.hpp>
#include <cassert>

CyclicPrefix::CyclicPrefix(size_t N, size_t cpLen, size_t ticksPerOutput) :
    m_len(cpLen),
    m_inputIdx(0),
    m_outputIdx(0),
    m_ticksPerOutput(ticksPerOutput),
    m_ticksSinceOutput(ticksPerOutput),
    m_outputReady(false),
    m_inputs(N),
    m_outputs(N + cpLen)
{
    assert (cpLen < N);
}

bool CyclicPrefix::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT);
    m_inputs[m_inputIdx].setFormat(data.fc);
    m_inputs[m_inputIdx++] = data.fc;
    return true;
}

bool CyclicPrefix::output(filter_io_t &data)
{
    if (m_outputIdx >= m_outputs.size()) {
        m_outputIdx = 0;
        m_outputReady = false;
    }
    if (m_outputReady) {
        if (m_ticksSinceOutput >= m_ticksPerOutput) {
            m_ticksSinceOutput = 0;
            data.type = IO_TYPE_COMPLEX_FIXPOINT;
            data.fc.setFormat(m_outputs[m_outputIdx]);
            data.fc = m_outputs[m_outputIdx];
            m_outputIdx++;
            return true;
        }
    }
    return false;
}

void CyclicPrefix::tick()
{
    if (m_inputIdx == m_inputs.size()) {
        m_inputIdx = 0;
        assert(m_outputIdx == 0);
        //Fill up the prefix
        for (size_t i = m_inputs.size() - m_len; i < m_inputs.size(); i++) {
            size_t outIdx = i - (m_inputs.size() - m_len);
            //grab the last m_len inputs and store them at the top of the output
            m_outputs[outIdx].setFormat(m_inputs[i]);
            m_outputs[outIdx] = m_inputs[i];
        }
        //Fill the rest
        for (size_t i = 0; i < m_inputs.size(); i++) {
            m_outputs[i + m_len].setFormat(m_inputs[i]);
            m_outputs[i + m_len] = m_inputs[i];
        }
        m_outputReady = true;
    }
    m_ticksSinceOutput++;
}
