/**
 * @file depuncture.hpp Perform the inverse of puncturing/repetition for LDPC
 * decoding.
 */
#pragma once

#include <core/filter_chain_element.hpp>
#include <filters/demapper.hpp>
#include <utils/mcs.hpp>
#include <types/circularbuffer.hpp>

/**
 * Perform the inverse of puncturing/repetition for LDPC decoding.
 * More information about this process can be found starting at page 165 of
 * Next Generation Wireless LANs. See:
 **https://books.google.com/books?id=hJ1cAwAAQBAJ&lpg=PP1&dq=Next%20Generation%20Wireless%20LANs&pg=PA165#v=snippet&q=LDPC%20Encoding%20Process&f=false
 * @see Puncture
 */
class Depuncture : public FilterChainElement
{
public:

    Depuncture(MCS mcs);
    bool output(filter_io_t& data)      override;
    void tick(void)                     override;

private:

    bool handlePuncture(filter_io_t& data);
    bool handleRepetition(filter_io_t& data);

    MCS m_mcs;
    size_t m_outBitCount;
    size_t m_outCodeWordCount;
    bool   m_doPuncture;
};
