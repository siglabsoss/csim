
#include <core/radio_s.hpp>
#include <types/filter_io.hpp>
#include <utility>

#include <cassert>


RadioS::RadioS(const radio_config_t &config, FilterChain &modChain, FilterChain &demodChain) :
    m_id(config.id),
    m_position(config.position),
    m_txChain(std::move(modChain)),
    m_rxChain(std::move(demodChain))
{
    m_txChain.init();
    m_rxChain.init();
}

radio_id_t RadioS::getId() const
{
    return m_id;
}

bool RadioS::rxByte(uint8_t &byte)
{
    filter_io_t data;
    bool didRx = m_rxChain.output(data);
    if (didRx) {
        assert(data.type == IO_TYPE_BYTE); //sanity check on the demodulation filter chain output
        byte = data.byte;
    }
    return didRx;
}

bool RadioS::txByte(const uint8_t &byte)
{
    filter_io_t data;
    data.type = IO_TYPE_BYTE;
    data.byte = byte;
    return m_txChain.input(data);
}

bool RadioS::rxWave(const ComplexDouble &sample_in)
{
    filter_io_t data;
    data.type = IO_TYPE_COMPLEX_DOUBLE;
    data.rf = sample_in;
    return m_rxChain.input(data);
}

bool RadioS::txWave(ComplexDouble &sample_out)
{
    filter_io_t data;
    //A properly formed modulation filter chain will always
    //have an output, but we check anyway since it's useful for testing
    bool didTx = m_txChain.output(data);
    if (didTx) {
        assert(data.type == IO_TYPE_COMPLEX_DOUBLE || data.type == IO_TYPE_COMPLEX_FIXPOINT); //sanity check on the modulation filter chain output
        sample_out = data.toComplexDouble();
    }
    return didTx;
}

Vector2d  RadioS::getPosition() const
{
    return m_position;
}

void RadioS::tick()
{
    m_txChain.tick();
    m_rxChain.tick();
}
