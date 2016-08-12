#pragma once

#include <iostream>
#include <cassert>

#include <core/filter_probe.hpp>

class TickCountTrigger : public FilterProbe
{
public:
    virtual ~TickCountTrigger() {}
    TickCountTrigger(const std::string &name, uint32_t tickCount, uint32_t numTriggers, uint32_t tickOffset) :
        FilterProbe(name, tickCount),
        m_count(0),
        m_triggers(0),
        m_offsetReached(false),
        m_tickCount(tickCount),
        m_numTriggers(numTriggers),
        m_tickOffset(tickOffset)
    {

    }

    void tick(void) override
    {
        m_count++;
        if (!m_offsetReached) {
            if (m_count >= m_tickOffset) {
                m_offsetReached = true;
                m_count = 0;
            }
            return;
        }

        if (m_triggers < m_numTriggers && m_count >= m_tickCount) {
            m_count = 0;
            std::cout << "TickCountTriggered!" << std::endl;
            std::ostringstream title;
            title << " - " << m_triggers + 1;
            nplot(title.str());
            m_triggers++;
        }
    }

private:
    size_t m_count;
    uint32_t m_triggers;
    bool    m_offsetReached;

    uint32_t m_tickCount;
    uint32_t m_numTriggers;
    uint32_t m_tickOffset;
};


