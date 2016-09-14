#pragma once

#include <core/filter_chain_element.hpp>
#include <filters/modulator.hpp>
#include <filters/hard_demod.hpp>

class SoftDemod : public HardDemod
{
public:
//    bool output(filter_io_t &data) override;
    void tick(void) override;
    SoftDemod(Modulator::mod_scheme_t scheme);
//    HardDemod(Modulator::mod_scheme_t scheme, double theta);
//
//    bool input(const filter_io_t &data) override;
//
    bool output(filter_io_t &data) override;
//
//    void tick(void) override;
//
private:
    void    queueLLR(double llr);
    bool    dequeueLLR(double &llr);
//    static double angleDiff(double a, double b);
//
    std::queue<double>            m_llrs;
//    filter_io_t                 m_value;
//    bool                        m_inputValid;
//    double                      m_theta;
//    constellation_map_t         m_constellations; //mapping of symbol -> constellation vector
//    size_t                      m_bitsPerSymbol;
};
