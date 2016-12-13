#pragma once

#include <core/filter_chain_element.hpp>
#include <filters/demapper.hpp>
#include <utils/mcs.hpp>
#include <types/circularbuffer.hpp>

class Depuncture : public FilterChainElement
{
public:
    Depuncture(MCS mcs);
    bool output(filter_io_t &data)      override;
    void tick(void)                     override;
private:
    bool handlePuncture(filter_io_t &data);
    bool handleRepetition(filter_io_t &data);

    MCS m_mcs;
    size_t m_outBitCount;
    size_t m_outCodeWordCount;
    bool m_doPuncture;
};

