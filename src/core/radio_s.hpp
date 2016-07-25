/**
 * Class for managing a simulated Signal Labs radio dubbed "RadioS".
 */
#pragma once

#include <interfaces/abstract_radio.hpp>

#include <core/filter_chain.hpp>

struct radio_config_t
{
    Vector2d position;
    //XXX additional radio configurations go here
};

class RadioS : public AbstractRadio
{
public:

    //Demodulation
    bool         rxWave(const std::complex<double> &sample_in) override;
    bool         rxByte(uint8_t &byte) override;

    //Modulation
    bool         txByte(const uint8_t &byte) override;
    bool         txWave(std::complex<double> &sample_out) override;

    //Other
    Vector2d     getPosition() const override;
    void         tick() override;

    RadioS(const radio_config_t &config, FilterChain modChain, FilterChain demodChain);

private: //members
    Vector2d                m_position;
    FilterChain             m_mod;
    FilterChain             m_demod;
};
