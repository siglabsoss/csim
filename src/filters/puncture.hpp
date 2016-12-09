#pragma once

#include <core/filter_chain_element.hpp>
#include <types/circularbuffer.hpp>
#include <utils/mcs.hpp>
#include <vector>

class Puncture : public FilterChainElement
{
public:
    Puncture(MCS mcs);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data)      override;
    void tick(void)                     override;
private:
    bool handlePuncture(filter_io_t &data);
    bool handleRepetition(filter_io_t &data);

    MCS m_mcs;
    std::vector<bool> m_buffer;
    bool m_doPuncture;
    CircularBuffer<bool> m_fifo;
    size_t m_outBitCount;
    size_t m_outCodeWordCount;
};

