/**
 * Class for managing a simulated Signal Labs radio dubbed "RadioS".
 */
#pragma once

#include <interfaces/abstract_radio.hpp>

#include <core/filter_chain.hpp>
#include <types/complexdouble.hpp>

typedef uint32_t radio_id_t;

struct radio_config_t
{
    Vector2d position;
    radio_id_t id;
};

class RadioS : public AbstractRadio
{
public:

    //Demodulation
    bool         rxWave(const ComplexDouble &sample_in) override;
    bool         rxByte(uint8_t &byte) override;

    //Modulation
    bool         txByte(const uint8_t &byte) override;
    bool         txWave(ComplexDouble &sample_out) override;

    //Other
    Vector2d     getPosition() const override;
    radio_id_t   getId() const;
    void         tick() override;

    RadioS(const radio_config_t &config, FilterChain &modChain, FilterChain &demodChain);

private: //members
    radio_id_t              m_id;
    Vector2d                m_position;
    FilterChain             m_mod;
    FilterChain             m_demod;
};
