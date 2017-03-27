#pragma once

#include <core/filter_chain_element.hpp>
#include <utils/mcs.hpp>

class FrameSync : public FilterChainElement
{
public:
    FrameSync(size_t N, size_t cpLen, size_t sampleDelay, MCS mcs);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;
private:
    enum sync_state_t {
        STATE_WAIT_FOR_NONZERO = 0,
        STATE_WAIT_FOR_FRAME,
        STATE_DROP_SHORT_PREAMBLE_PREFIX,
        STATE_DROP_LONG_PREAMBLE_PREFIX,
        STATE_DROP_PREFIX,
        STATE_DROP_SHORT_PREAMBLE,
        STATE_DROP_LONG_PREAMBLE,
        STATE_PASS_SYMBOL
    };
    sync_state_t m_state;
    size_t m_Nfft;
    size_t m_cpLen;
    size_t m_sampleCount;
    size_t m_totalCount;
    bool m_gotInput;
    filter_io_t m_sample;
    size_t m_sampleDelay;
    size_t m_symbolCount;
    MCS    m_mcs;
};
