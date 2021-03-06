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
    NoiseElement(double variance);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;

private:
    ComplexDouble m_input;
    ComplexGaussianNoise m_noiseGenerator;
    bool    m_inputValid;
};


