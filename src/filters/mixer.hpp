#pragma once

#include <iostream>
#include <cassert>

#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>

class Mixer : public FilterChainElement
{
public:
    virtual ~Mixer();
    Mixer(uint32_t ticksPerPeriod, bool upMix);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;

private:
    ComplexDouble m_carrier;
    ComplexDouble m_input;

    size_t m_count;
    uint32_t m_ticksPerPeriod;
    bool    m_inputValid;
    bool    m_upMix;
};


