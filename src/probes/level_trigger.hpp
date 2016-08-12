#pragma once

#include <iostream>
#include <cassert>

#include <core/filter_probe.hpp>

class LevelTrigger : public FilterProbe
{
public:

    virtual ~LevelTrigger();
    LevelTrigger(const std::string &name, size_t sampleCount, double level);
    void tick(void) override;

private:
    bool    didTrigger(double level) const;
    double  m_level;
    bool    m_didTrigger;
};


