#pragma once

#include <iostream>
#include <cassert>

#include <core/filter_probe.hpp>

class SampleCountTrigger : public FilterProbe
{
public:
    virtual ~SampleCountTrigger() {}
    SampleCountTrigger(const std::string &name, data_dump_t outType, uint32_t sampleCount, uint32_t numTriggers, uint32_t sampleOffset) :
        FilterProbe(name, sampleCount, outType),
        m_count(0),
        m_triggers(0),
        m_offsetReached(false),
        m_sampleCount(sampleCount),
        m_numTriggers(numTriggers),
        m_sampleOffset(sampleOffset)
    {

    }

    void tick(void) override
    {
        if (m_validInput && m_didReceiveNonzero) {
            m_count++;
        }

        if (!m_offsetReached) {
            if (m_count > m_sampleOffset) {
                m_offsetReached = true;
                m_count = 1;
            } else {
                return;
            }
        }

        if (m_triggers < m_numTriggers && m_count >= m_sampleCount) {
            m_count = 0;
            std::ostringstream title;
            title << m_triggers + 1;
            trigger(title.str());
            m_triggers++;
            std::cout << m_name << ": Sample count probe trigger #" << m_triggers << std::endl;
        }
    }

private:
    size_t m_count;
    uint32_t m_triggers;
    bool    m_offsetReached;

    uint32_t m_sampleCount;
    uint32_t m_numTriggers;
    uint32_t m_sampleOffset;
};


