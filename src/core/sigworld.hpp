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

    bool sendBit(radio_id_t id, bool byte);
    bool sendBits(radio_id_t id, const std::vector<bool> &bits);
    void didReceiveByte(std::function< void(radio_id_t, bool) > callback);
    void setExternalDataSource(std::function< void(const RadioS *current, ComplexDouble &data) > source);
    void setExternalDataDestination(std::function< void(const RadioS *current, const ComplexDouble &data) > destination);
private:
    RadioSet *                                  m_radioSet;
    std::function< void(radio_id_t, bool) >  m_rxCallback;
    std::function< void(const RadioS *current, ComplexDouble &data) > m_externalSampleSource;
    std::function< void(const RadioS *current, const ComplexDouble &data) > m_externalSampleDestination;
};
