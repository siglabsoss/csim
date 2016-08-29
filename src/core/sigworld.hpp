#pragma once

#include <core/radio_set.hpp>
#include <core/publisher.hpp>
#include <memory>

class SigWorld
{
public:
    SigWorld();
    void init(RadioSet *rs);
    void tick();
    void reset();

    bool sendByte(radio_id_t id, uint8_t byte);
    void didReceiveByte(std::function< void(radio_id_t, uint8_t) > callback);
    void setExternalDataSource(std::function< void(const RadioS *current, ComplexDouble &data) > source);
    void setExternalDataDestination(std::function< void(const RadioS *current, const ComplexDouble &data) > destination);
private:
    RadioSet *                                  m_radioSet;
    std::function< void(radio_id_t, uint8_t) >  m_rxCallback;
    std::function< void(const RadioS *current, ComplexDouble &data) > m_externalSampleSource;
    std::function< void(const RadioS *current, const ComplexDouble &data) > m_externalSampleDestination;
};
