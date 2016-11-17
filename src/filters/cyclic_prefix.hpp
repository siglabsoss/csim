#pragma once

#include <core/filter_chain_element.hpp>
#include <vector>

class CyclicPrefix : public FilterChainElement
{
public:
    CyclicPrefix(size_t N, size_t cpLen, size_t ticksPerOutput);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;
private:
    size_t m_len;
    size_t m_inputIdx;
    size_t m_outputIdx;
    size_t m_ticksPerOutput;
    size_t m_ticksSinceOutput;
    bool   m_outputReady;
    std::vector<SLFixComplex> m_inputs;
    std::vector<SLFixComplex> m_outputs;
};
