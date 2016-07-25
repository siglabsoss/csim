#pragma once

#include <iostream>
#include <cassert>

#include <core/filter_chain_element.hpp>

class SineWave : public FilterChainElement
{
public:
    virtual ~SineWave() {}
    SineWave(double ticksPerPeriod) :
        m_ticksPerPeriod(ticksPerPeriod),
        m_count(0)
    {
        m_output.type = IO_TYPE_COMPLEX_DOUBLE;
    }
    bool input(const filter_io_t &data) override
    {
        //don't care about input
        return true;
    }

    bool output(filter_io_t &data) override
    {
        data = m_output;
        return true;
    }

    void tick(void) override
    {
        double theta = (((2 * M_PI) / m_ticksPerPeriod) * m_count);
        m_output.rf = std::complex<double>(cos(theta), sin(theta));
        m_count++;
        if (m_count > m_ticksPerPeriod) {
            m_count = 0;
        }
    }
private:
    double m_ticksPerPeriod;
    filter_io_t m_output;
    size_t m_count;

};


