
#include "radio_s.hpp"
#include "dummy_block_element.hpp"

RadioS::RadioS(Vector2d position) :
    m_position(position),
    m_mod(new DummyBlockElement(
          new DummyBlockElement(
          new DummyBlockElement(
          nullptr)))),
    m_demod(new DummyBlockElement(nullptr))
{

}

bool RadioS::rxByte(uint8_t &byte)
{
    block_io_t data;
    bool didRx = m_demod.output(data);
    if (didRx) {
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
        sample_out = data.rf;
    }
    return didRx;
}

Vector2d  RadioS::getPosition() const
{
    return m_position;
}

void RadioS::tick() {

}
