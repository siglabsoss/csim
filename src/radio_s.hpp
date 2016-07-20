/**
 * Class for managing a simulated Signal Labs radio dubbed "RadioS".
 */
#pragma once

#include "abstract_radio.hpp"
#include "filter_chain.hpp"

class RadioS : public AbstractRadio
{
public:
    RadioS(const Vector2d position, FilterChain &modChain, FilterChain &demodChain);

    //Demodulation
    bool         rxWave(const std::complex<double> &sample_in) override;
    bool         rxByte(uint8_t &byte) override;

    //Modulation
    bool         txByte(const uint8_t &byte) override;
    bool         txWave(std::complex<double> &sample_out) override;

    Vector2d     getPosition() const override;

    void         tick() override;
private:
    Vector2d                m_position;
    FilterChain &           m_mod;
    FilterChain &           m_demod;
};
