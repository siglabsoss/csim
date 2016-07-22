#pragma once

#include <core/radio_set.hpp>

class SigWorld
{
public:
    SigWorld();
    void addRadio(radio_config_t &config);
    void init();
    void tick();
private:
    RadioSet    m_radioSet;
};
