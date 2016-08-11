#pragma once

#include <iostream>
#include <cassert>

#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>

class Mixer : public FilterChainElement
{
public:
    virtual ~Mixer();
    Mixer(uint32_t fs, int32_t hz);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;

private:
    uint32_t m_fs;
    filter_io_t m_output;
    filter_io_t m_hold;
    size_t m_count;
    int32_t m_ticksPerPeriod;
    std::vector<std::complex<double> > m_vec;
};


