#pragma once

#include <iostream>
#include <cassert>

#include <core/filter_chain_element.hpp>
#include <mathlib/complex_gaussian_noise.hpp>
#include <utils/plotter.hpp>
#include <vector>

class NoiseElement : public FilterChainElement
{
public:
    virtual ~NoiseElement();
    NoiseElement(double ebn0);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;

private:
    filter_io_t m_input;
    ComplexGaussianNoise m_noiseGenerator;
    bool    m_inputValid;
    double m_scalar;
};


