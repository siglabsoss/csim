#pragma once

#include <core/filter_chain_element.hpp>
#include <cassert>

class CyclicPrefix : public FilterChainElement
{
public:
    CyclicPrefix(size_t N, size_t cpLen);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;
private:
    size_t m_N;
    size_t m_len;
    size_t m_inputIdx;
    size_t m_outputIdx;
    size_t m_ticksSinceInput;
    size_t m_ticksPerOutput;
    std::vector<SLFixComplex> m_inputs;
    std::vector<SLFixComplex> m_outputs;
};

CyclicPrefix::CyclicPrefix(size_t N, size_t cpLen) :
    m_N(N),
    m_len(cpLen),
    m_inputIdx(0),
    m_outputIdx(0),
    m_ticksSinceInput(0),
    m_ticksPerOutput(0),
    m_inputs(N),
    m_outputs(N)
{
    assert (cpLen < N);
}

bool CyclicPrefix::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT);
    m_inputs[m_inputIdx].setFormat(data.fc);
    m_inputs[m_inputIdx++] = data.fc;
    m_ticksSinceInput = 0;
    return true;
}

bool CyclicPrefix::output(filter_io_t &data)
{
    bool outputReady = (m_inputIdx == m_N) || (m_outputIdx > 0 && m_ou)
}

void CyclicPrefix::tick()
{
    if (m_inputIdx == m_N) {
        assert(m_outputIdx == 0);
        for (size_t i = 0; i < m_inputs.size(); i++) {
            m_outputs[i].setFormat(m_inputs[i]);
            m_outputs[i] = m_inputs[i];
        }
    }
    m_ticksSinceInput++;
}
