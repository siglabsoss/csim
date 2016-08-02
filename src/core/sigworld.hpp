#pragma once

#include <core/radio_set.hpp>
#include <core/publisher.hpp>
#include <memory>

class SigWorld
{
public:
    SigWorld();
    void addRadio(std::function< std::unique_ptr<RadioS>() > radioFactory);
    void init();
    void tick();
private:
    RadioSet    m_radioSet;
};
