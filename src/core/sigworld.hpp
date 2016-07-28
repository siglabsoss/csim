#pragma once

#include <core/radio_set.hpp>
#include <core/publisher.hpp>
#include <memory>

class SigWorld
{
public:
    SigWorld();
    void addRadio(std::unique_ptr<RadioS> (radioFactory)(const radio_config_t &config), radio_config_t &config);
    void init();
    void tick();
private:
    RadioSet    m_radioSet;
};
