#pragma once

#include <core/radio_set.hpp>
#include <core/publisher.hpp>
#include <memory>

class SigWorld
{
public:
    SigWorld();
    void init();
    void tick();

    void addRadio(std::function< std::unique_ptr<RadioS>() > radioFactory);
    bool sendByte(radio_id_t id, uint8_t byte);
    void didReceiveByte(std::function< void(radio_id_t, uint8_t) > callback);
private:
    RadioSet    m_radioSet;
    std::function< void(radio_id_t, uint8_t) > m_rxCallback;
};
