#pragma once

#include <iostream>
#include <cassert>

#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>

class TriggerPlot : public FilterChainElement
{
public:
    virtual ~TriggerPlot() {}
    TriggerPlot(uint32_t trigger_count, uint32_t max_triggers) :
        FilterChainElement(std::string("TriggerPlot")),
        m_count(0),
        m_trigger_count(trigger_count),
        m_max_triggers(max_triggers),
        m_triggers(0),
        m_p()
    {

    }
    bool input(const filter_io_t &data) override
    {
        m_count++;
        m_vec.push_back(data.rf);
        return true;
    }

    bool output(filter_io_t &data) override
    {
        return false;
    }

    void tick(void) override
    {
        if (m_triggers < m_max_triggers && m_count >= m_trigger_count) {
            m_count = 0;

            std::ostringstream title;
            title << "TriggerPlot " << m_triggers;
            m_p.nplot(m_vec, title.str());

            m_triggers++;
            m_vec.clear();
        }
    }
private:
    size_t m_count;
    uint32_t m_trigger_count;
    uint32_t m_max_triggers;
    uint32_t m_triggers;
    plotter m_p;
    std::vector<ComplexDouble > m_vec;

};


