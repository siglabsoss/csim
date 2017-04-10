/**
 * @file Perform puncturing and/or repetition for LDPC encoding.
 */
#pragma once

#include <core/filter_chain_element.hpp>
#include <types/circularbuffer.hpp>
#include <utils/mcs.hpp>
#include <vector>

/**
 * Perform puncturing. Puncturing/repetition is part of the LDPC encoding
 * process for OFDM communication systems.
 * More information about this process can be found starting at page 165 of
 * Next Generation Wireless LANs. See:
 **https://books.google.com/books?id=hJ1cAwAAQBAJ&lpg=PP1&dq=Next%20Generation%20Wireless%20LANs&pg=PA165#v=snippet&q=LDPC%20Encoding%20Process&f=false
 */
class Puncture : public FilterChainElement
{
public:

    Puncture(MCS mcs);
    bool output(filter_io_t& data)      override;
    void tick(void)                     override;

private:

    bool handlePuncture(filter_io_t& data);
    bool handleRepetition(filter_io_t& data);

    MCS m_mcs;
    std::vector<bool> m_buffer;
    bool   m_doPuncture;
    size_t m_outBitCount;
    size_t m_outCodeWordCount;
};
