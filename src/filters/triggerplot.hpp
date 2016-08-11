#pragma once

#include <iostream>
#include <cassert>

#include <core/filter_probe.hpp>

class TriggerPlot : public FilterProbe
{
public:
    virtual ~TriggerPlot() {}
    TriggerPlot(uint32_t trigger_count, uint32_t num_triggers, uint32_t offset) :
        FilterProbe(trigger_count),
        m_count(0),
        m_trigger_count(trigger_count),
        m_num_triggers(num_triggers),
        m_triggers(0),
        m_offset(offset),
        m_nonZeroHit(false)
    {

    }

    void tick(void) override
    {
        if (m_validInput) {
            m_count++;
        }

        if (!m_nonZeroHit) {
            if (m_history[0].rf.real() > 0.0) {
                m_nonZeroHit = true;
                m_count = 0;
            }
            return;
        }

        if (m_triggers < m_num_triggers && m_count >= m_trigger_count) {
            m_count = 0;

            std::ostringstream title;
            title << "TriggerPlot " << m_triggers + 1;
            nplot(title.str());
            m_triggers++;
        }
    }
private:
    size_t m_count;
    uint32_t m_trigger_count;
    uint32_t m_num_triggers;
    uint32_t m_triggers;

    uint32_t m_offset;
    bool    m_nonZeroHit;
};


