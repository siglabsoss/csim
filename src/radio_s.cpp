
#include "radio_s.hpp"
#include "dummy_filter_element.hpp"
#include <cassert>

RadioS::RadioS(Vector2d position, FilterChain &modChain, FilterChain &demodChain) :
    m_position(position),
    m_mod(modChain),
    m_demod(demodChain)
{}

bool RadioS::rxByte(uint8_t &byte)
{
    block_io_t data;
    bool didRx = m_demod.output(data);
    if (didRx) {
        assert(data.type == IO_TYPE_BYTE); //sanity check on the demodulation filter chain output
        byte = data.byte;
    }
    return didRx;
}

bool RadioS::txByte(const uint8_t &byte)
{
    block_io_t data;
    data.type = IO_TYPE_BYTE;
    data.byte = byte;
    return m_mod.input(data);
}

bool RadioS::rxWave(const std::complex<double> &sample_in)
{
    block_io_t data;
    data.type = IO_TYPE_COMPLEX_DOUBLE;
    data.rf = sample_in;
    return m_demod.input(data);
}

bool RadioS::txWave(std::complex<double> &sample_out)
{
    block_io_t data;
    bool didRx = m_mod.output(data);
    if (didRx) {
        assert(data.type == IO_TYPE_COMPLEX_DOUBLE); //sanity check on the modulation filter chain output
        sample_out = data.rf;
    }
    return didRx;
}

Vector2d  RadioS::getPosition() const
{
    return m_position;
}

void RadioS::tick()
{

}