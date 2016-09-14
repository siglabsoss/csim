#pragma once

#include <core/filter_chain_element.hpp>
#include <filters/modulator.hpp>
#include <filters/hard_demod.hpp>

class SoftDemod : public HardDemod
{
public:
    void tick(void) override;
    SoftDemod(Modulator::mod_scheme_t scheme, double noise_variance = 1.0);
    bool output(filter_io_t &data) override;

private:
    std::queue<double>            m_llrs;
    double                        m_noise_variance;
};
