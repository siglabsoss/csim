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
        OUTPUT_PREAMBLE,
        OUTPUT_SYMBOLS
    };
    SubcarrierMapper(MCS    mcs,
                     size_t pilotSpacing,
                     size_t initialPilotOfset,
                     size_t symbolRep);
    bool output(filter_io_t& data) override;
    void tick() override;

private:

    void         initializePreamble();
    size_t       getNumPilotTones() const;

    SLFixComplex getNextPreambleSymbol(bool& finished);
    size_t       subcarrierIdxToFFTIdx(ssize_t subcarrierIdx) const;

    MCS m_mcs;
    size_t  m_pilotSpacing;
    size_t  m_initialPilotOffset;
    size_t  m_symbolRep;
    state_t m_state;
    filter_io_t m_output;
    bool m_shouldOutput;

    std::vector<SLFixComplex> m_preamble;
    size_t                    m_preambleIdx;
    size_t                    m_subSymbolCount;
};
