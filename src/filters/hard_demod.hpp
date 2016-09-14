#pragma once

#include <core/filter_chain_element.hpp>
#include <filters/modulator.hpp>

class HardDemod : public FilterChainElement
{
public:
    HardDemod(Modulator::mod_scheme_t scheme, double theta);

    bool input(const filter_io_t &data) override;

    bool output(filter_io_t &data) override;

    void tick(void) override;

protected:
    void    queueSymbol(symbol_t symbol);
    bool    dequeueByte(uint8_t &byte);
    static double angleDiff(double a, double b);

    std::queue<bool>            m_bits;
    filter_io_t                 m_value;
    bool                        m_inputValid;
    double                      m_theta;
    constellation_map_t         m_constellations; //mapping of symbol -> constellation vector
    size_t                      m_bitsPerSymbol;
};
