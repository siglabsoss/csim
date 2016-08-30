#pragma once

#include <core/filter_chain_element.hpp>
#include <filters/biquad.hpp>

class BiquadIIR : public FilterChainElement
{
public:
    BiquadIIR(size_t numStages);

    bool input(const filter_io_t &data) override;

    bool output(filter_io_t &data) override;

    void tick(void) override;

private:
    std::vector<Biquad>         m_stages;
    bool                        m_outputReady;
    filter_io_t                 m_sample;
};
