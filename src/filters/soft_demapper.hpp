#pragma once

#include <core/filter_chain_element.hpp>
#include <filters/mapper.hpp>
#include <filters/hard_demapper.hpp>

class SoftDemapper : public HardDemapper
{
public:
    void tick(void) override;
    SoftDemapper(Mapper::constellation_set_t scheme);
    bool output(filter_io_t &data) override;

private:
    static double calcLLRIncrement(const ComplexDouble &rxSymbol, const ComplexDouble &constellation, double variance);
    std::queue<double>            m_llrs;

    double m_awgnVariance;
};
