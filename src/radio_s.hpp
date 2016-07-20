/**
 * Class for managing a simulated Signal Labs radio dubbed "RadioS".
 */
#pragma once

#include "abstract_radio.hpp"
#include "block_chain.hpp"

class RadioS : public AbstractRadio
{
public:
    RadioS(const Vector2d position);
    bool         rxByte(uint8_t &byte) override;
    bool         txByte(const uint8_t &byte) override;

    bool         rxWave(const std::complex<double> &sample_in) override;
    bool         txWave(std::complex<double> &sample_out) override;

    Vector2d     getPosition() const override;

    void         tick() override;
private:
    Vector2d                m_position;
    BlockChain              m_mod;
    BlockChain              m_demod;
};
