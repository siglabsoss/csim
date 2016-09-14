#include <filters/modulator.hpp>

#include <core/parameters.hpp>

#include <cassert>

Modulator::Modulator(unsigned int ticksPerSymbol, mod_scheme_t scheme = MOD_SCHEME_BPSK) :
    FilterChainElement("Modulator"),
    m_constellations(),
    m_bitsPerSymbol(0),
    m_inputBuffer(),
    m_tickCount(0),
    m_ticksPerSymbol(ticksPerSymbol)
{
    m_tickCount = m_ticksPerSymbol; //set to trigger an output update on first iteration
    switch(scheme) {
        case MOD_SCHEME_BPSK:
            m_constellations = getBPSKConstellations();
            m_bitsPerSymbol = 1;
            break;
        case MOD_SCHEME_QPSK:
            m_constellations = getQPSKConstellations();
            m_bitsPerSymbol = 2;
            break;
        case MOD_SCHEME_8PSK:
            m_constellations = get8PSKConstellations();
            m_bitsPerSymbol = 3;
            break;
        case MOD_SCHEME_QAM16:
            m_constellations = getQAM16Constellations();
            m_bitsPerSymbol = 4;
            break;
        case MOD_SCHEME_NULL:
        default:
            break;
    }
}

bool Modulator::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_BYTE);
    //make sure we have enough space left to store the next symbol
    if (INPUT_BUFFER_BITS_MAX - m_inputBuffer.size() < sizeof(data.byte) * 8) {
        return false;
    }
    for (size_t i = 0; i < sizeof(data.byte) * 8; i++) {
        m_inputBuffer.push(data.byte & (1 << i));//XXX confirm left to right of vice versa
    }
    return true;
}
bool Modulator::output(filter_io_t &data)
{
    if (m_tickCount == 1) {
        data = m_output;
        return true;
    }
    return false;
}
void Modulator::tick(void)
{
    if (m_tickCount >= m_ticksPerSymbol) {
        symbol_t symbol = NULL_SYMBOL;
        if (m_inputBuffer.size() >= m_bitsPerSymbol) { //we have real input to modulate
            symbol = getNextSymbol();
            m_output = m_constellations.at(symbol);
        } else {
            m_output = constellation_t(0.0, 0.0);
        }
        m_tickCount = 0;
    }
    m_tickCount++;
}

symbol_t Modulator::getNextSymbol()
{
    symbol_t symbol = 0;
    for (size_t i = 0; i < m_bitsPerSymbol; i++) { //XXX confirm direction
        symbol_t nextBit = !!m_inputBuffer.front();
        m_inputBuffer.pop();
        symbol |= (nextBit << i);
    }
    return symbol;
}

/* Static constellation generators */

constellation_map_t Modulator::getBPSKConstellations()
{
    constellation_map_t constellations;
    constellations[0b0] = constellation_t(-1.0, 0.0);
    constellations[0b1] = constellation_t( 1.0, 0.0);

    return constellations;
}
constellation_map_t Modulator::getQPSKConstellations()
{
    constellation_map_t constellations;
    constellations[0b00] = constellation_t(-M_SQRT1_2, -M_SQRT1_2);
    constellations[0b01] = constellation_t(-M_SQRT1_2,  M_SQRT1_2);
    constellations[0b10] = constellation_t( M_SQRT1_2, -M_SQRT1_2);
    constellations[0b11] = constellation_t( M_SQRT1_2,  M_SQRT1_2);
    return constellations;
}
constellation_map_t Modulator::get8PSKConstellations()
{
    constellation_map_t constellations;
    constellations[0b000] = constellation_t(-M_SQRT1_2, -M_SQRT1_2);
    constellations[0b001] = constellation_t(-1.0,  0.0);
    constellations[0b011] = constellation_t(-M_SQRT1_2,  M_SQRT1_2);
    constellations[0b010] = constellation_t( 0.0,  1.0);
    constellations[0b110] = constellation_t( M_SQRT1_2,  M_SQRT1_2);
    constellations[0b111] = constellation_t( 1.0,  0.0);
    constellations[0b101] = constellation_t( M_SQRT1_2, -M_SQRT1_2);
    constellations[0b100] = constellation_t( 0.0, -1.0);

    return constellations;
}
constellation_map_t Modulator::getQAM16Constellations()
{
    //https://upload.wikimedia.org/wikipedia/commons/9/90/QAM16_Demonstration.gif
    constellation_map_t constellations;
    //Top Right
    constellations[0b0000] = constellation_t( M_SQRT1_2/3,  M_SQRT1_2/3);
    constellations[0b0001] = constellation_t( M_SQRT1_2  ,  M_SQRT1_2/3);
    constellations[0b0010] = constellation_t( M_SQRT1_2/3,  M_SQRT1_2  );
    constellations[0b0011] = constellation_t( M_SQRT1_2  ,  M_SQRT1_2  );

    //Bottom Right
    constellations[0b0100] = constellation_t( M_SQRT1_2/3, -M_SQRT1_2/3);
    constellations[0b0101] = constellation_t( M_SQRT1_2/3, -M_SQRT1_2  );
    constellations[0b0110] = constellation_t( M_SQRT1_2,   -M_SQRT1_2/3);
    constellations[0b0111] = constellation_t( M_SQRT1_2  , -M_SQRT1_2  );

    //Top Left
    constellations[0b1000] = constellation_t(-M_SQRT1_2/3,  M_SQRT1_2/3);
    constellations[0b1001] = constellation_t(-M_SQRT1_2/3,  M_SQRT1_2  );
    constellations[0b1010] = constellation_t(-M_SQRT1_2,    M_SQRT1_2/3);
    constellations[0b1011] = constellation_t(-M_SQRT1_2  ,  M_SQRT1_2  );

    //Bottom Left
    constellations[0b1100] = constellation_t(-M_SQRT1_2/3, -M_SQRT1_2/3);
    constellations[0b1101] = constellation_t(-M_SQRT1_2,   -M_SQRT1_2/3);
    constellations[0b1110] = constellation_t(-M_SQRT1_2/3, -M_SQRT1_2  );
    constellations[0b1111] = constellation_t(-M_SQRT1_2  , -M_SQRT1_2  );

    return constellations;
}
