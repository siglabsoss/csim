#pragma once

#include <core/filter_chain_element.hpp>
#include <filters/mapper.hpp>
#include <utils/plotter.hpp>

class Demapper : public FilterChainElement
{
public:
    Demapper(Mapper::constellation_set_t scheme, bool hard);
    ~Demapper();

    bool input(const filter_io_t &data) override;

    bool output(filter_io_t &data) override;

    void tick(void) override;

private:
    static double calcLLRIncrement(const ComplexDouble &rxSymbol, const ComplexDouble &constellation, double variance);

    filter_io_t                 m_value;
    bool                        m_inputValid;
    constellation_map_t         m_constellations; //mapping of symbol -> constellation vector
    size_t                      m_bitsPerSymbol;
    bool                        m_hard; //hard decision (vs. soft decision)
    double                      m_awgnVariance;
    std::queue<double>          m_llrs;
};