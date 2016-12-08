#pragma once

#include <core/filter_chain_element.hpp>
#include <vector>

class ChannelEqualizer : public FilterChainElement
{
public:
    ChannelEqualizer(const std::vector<ComplexDouble> &Hf);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;
private:
    std::vector<ComplexDouble> m_Hf;
    SLFixComplex m_sample;
    size_t m_outIdx;
    bool gotInput;
};

