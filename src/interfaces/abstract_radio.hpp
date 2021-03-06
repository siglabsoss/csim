#pragma once

#include <Eigen/Dense>
#include <cstdint>

#include <types/complexdouble.hpp>

using Eigen::Vector2d;

/**
 * Radio interface definition
 */

class AbstractRadio
{
public:
    virtual ~AbstractRadio() {}

    //Demodulation
    virtual bool         rxWave(const ComplexDouble &sample_in) = 0;
    virtual bool         rxBit(bool &bit) = 0;

    //Modulation
    virtual bool         txBit(const bool &bit) = 0;
    virtual bool         txWave(ComplexDouble &sample_out) = 0;

    virtual Vector2d     getPosition() const = 0;

    virtual void        tick() = 0;
};
