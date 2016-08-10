#pragma once

#include <iostream>
#include <cassert>

#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>

class Mixer : public FilterChainElement
{
public:
    virtual ~Mixer() {}
    Mixer(uint32_t fs, int32_t hz) :
        FilterChainElement(std::string("Mixer")),
        m_fs(fs),
        m_count(0),
        m_ticksPerPeriod((int32_t)fs/hz)
    {
        m_output.type = IO_TYPE_COMPLEX_DOUBLE;
    }
    bool input(const filter_io_t &data) override
    {
    	m_hold = data;
        return true;
    }

    bool output(filter_io_t &data) override
    {
        data = m_output.rf * m_hold.rf;
        return true;
    }

    void tick(void) override
    {
        double theta = (((2 * M_PI) / m_ticksPerPeriod) * m_count);
        m_output.rf = std::complex<double>(cos(theta), sin(theta));
        m_count++;
        if (m_count > m_ticksPerPeriod) {
            m_count -= m_ticksPerPeriod;
        }
    }
private:
    uint32_t m_fs;
    filter_io_t m_output;
    filter_io_t m_hold;
    size_t m_count;
    int32_t m_ticksPerPeriod;
    std::vector<std::complex<double> > m_vec;

};


