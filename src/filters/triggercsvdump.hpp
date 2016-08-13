#pragma once

#include <iostream>
#include <cassert>
#include <iomanip>

#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>

class TriggerCSVDump : public FilterChainElement
{
public:
    virtual ~TriggerCSVDump() {}
    TriggerCSVDump(uint32_t trigger_count, uint32_t max_triggers) :
        FilterChainElement(std::string("TriggerCSVDump")),
        m_count(0),
        m_trigger_count(trigger_count),
        m_max_triggers(max_triggers),
        m_triggers(0),
        m_data()
    {
        clock_gettime(CLOCK_REALTIME, &m_epoc); // record the epoc time when class is created
    }
    bool input(const filter_io_t &data) override
    {
        m_count++;
        m_vec.push_back(data.rf);
        m_data = data;
        return true;
    }

    bool output(filter_io_t &data) override
    {
        data = m_data;
        return true;
    }

    void tick(void) override
    {
        if (m_triggers < m_max_triggers && m_count >= m_trigger_count) {
            m_count = 0;

            std::ostringstream filename;
            filename << m_epoc.tv_sec << "_" << m_triggers << ".csv"; // build a filename

            ofstream out3(filename.str().c_str());
            size_t i;
            for (i = 0; i < m_vec.size(); i++) {
                out3 << setw(11) << std::setfill(' ') << std::real(m_vec[i]) << "," ;
                out3 << setw(11) << std::setfill(' ') << std::imag(m_vec[i]) << std::endl;
            }

            std::cout << "Wrote " << filename.str() << std::endl;

            m_triggers++;
            m_vec.clear();
        }
    }
private:
    size_t m_count;
    uint32_t m_trigger_count;
    uint32_t m_max_triggers;
    uint32_t m_triggers;
    timespec m_epoc;
    filter_io_t m_data;
    std::vector<std::complex<double> > m_vec;
};


