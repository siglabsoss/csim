#pragma once

#include <core/filter_chain_element.hpp>
#include <filters/fft.hpp>
#include <utils/mcs.hpp>


#include <vector>

class SubcarrierMapper : public FilterChainElement
{
public:
    enum state_t {
        WAITING_FOR_INPUT = 0,
        OUTPUT_SHORT_PREAMBLE,
        OUTPUT_LONG_PREAMBLE,
        OUTPUT_SYMBOLS
    };
    SubcarrierMapper(MCS mcs);
    bool output(filter_io_t &data) override;
    void tick() override;

private:
    void         initializePreamble();

    SLFixComplex getNextShortPreambleSymbol(bool &finished);
    SLFixComplex getNextLongPreambleSymbol(bool &finished);

    MCS                       m_mcs;
    state_t                   m_state;
    filter_io_t               m_output;
    bool                      m_shouldOutput;

    std::vector<SLFixComplex> m_shortPreamble;
    std::vector<SLFixComplex> m_longPreamble;
    size_t                    m_preambleIdx;
    size_t                    m_subSymbolCount;
};
