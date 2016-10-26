#pragma once

#include <core/filter_chain_element.hpp>
#include <filters/mapper.hpp>
#include <filters/hard_demod.hpp>

class SoftDemod : public HardDemod
{
public:
    void tick(void) override;
    SoftDemod(Mapper::constellation_set_t scheme);
    bool output(filter_io_t &data) override;

private:
    static double calcLLRIncrement(const ComplexDouble &rxSymbol, const ComplexDouble &constellation);
    std::queue<double>            m_llrs;
};
