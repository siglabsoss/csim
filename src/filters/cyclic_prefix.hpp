#pragma once

#include <types/circularbuffer.hpp>
#include <core/filter_chain_element.hpp>
#include <utils/mcs.hpp>
#include <vector>

class CyclicPrefix : public FilterChainElement
{
public:
    CyclicPrefix(size_t N, size_t cpLen, size_t ticksPerOutput, MCS mcs);
    bool output(filter_io_t &data) override;
    void tick(void) override;
private:
    size_t m_len;
    size_t m_symbolLen;
    size_t m_inputIdx;
    size_t m_outputIdx;
    size_t m_ticksPerOutput;
    size_t m_ticksSinceOutput;
    bool   m_outputActive;
    std::vector<SLFixComplex> m_outputs;
};
