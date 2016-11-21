#pragma once

#include <core/filter_chain_element.hpp>

class FrameSync : public FilterChainElement
{
public:
    FrameSync(size_t N, size_t cpLen);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;
private:
    enum sync_state_t {
        STATE_WAIT_FOR_FRAME = 0,
        STATE_DROP_PREFIX,
        STATE_PASS_FRAME,
        STATE_POST_FRAME_DELAY
    };
    sync_state_t m_state;
    size_t m_Nfft;
    size_t m_cpLen;
    size_t m_sampleCount;
    bool m_gotInput;
    filter_io_t m_sample;
};


