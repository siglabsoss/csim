#pragma once

#include <core/filter_chain_element.hpp>
#include <utils/mcs.hpp>

class OFDMFrameSync : public FilterChainElement
{
public:

    OFDMFrameSync(size_t cpLen,
                  MCS    mcs);
    bool output(filter_io_t& data) override;
    void tick(void) override;

private:

    size_t m_Nfft;
    size_t m_cpLen;
    MCS    m_mcs;
    ComplexDouble m_P;
    double m_R;
};
