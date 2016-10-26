#include <filters/mapper.hpp>

#include <core/parameters.hpp>

#include <cassert>

Mapper::Mapper(unsigned int ticksPerSymbol, constellation_set_t scheme = CONST_SET_BPSK) :
    FilterChainElement("Mapper"),
    m_constellations(),
    m_bitsPerSymbol(0),
    m_inputBuffer(),
    m_tickCount(0),
    m_ticksPerSymbol(ticksPerSymbol)
{
    m_tickCount = m_ticksPerSymbol; //set to trigger an output update on first iteration
    switch(scheme) {
        case CONST_SET_BPSK:
            m_constellations = getBPSKConstellations();
            m_bitsPerSymbol = 1;
            break;
        case CONST_SET_QPSK:
            m_constellations = getQPSKConstellations();
            m_bitsPerSymbol = 2;
            break;
        case CONST_SET_8PSK:
            m_constellations = get8PSKConstellations();
            m_bitsPerSymbol = 3;
            break;
        case CONST_SET_QAM16:
            m_constellations = getQAM16Constellations();
            m_bitsPerSymbol = 4;
            break;
        case CONST_SET_NULL:
        default:
            break;
    }
}

bool Mapper::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_BYTE);
    //make sure we have enough space left to store the next symbol
    if (INPUT_BUFFER_BITS_MAX - m_inputBuffer.size() < sizeof(data.byte) * 8) {
        return false;
    }
    //queuing up least significant bit first
    std::cout << "mapper in: ";
    for (size_t i = 0; i < sizeof(data.byte) * 8; i++) {
        bool bit = (data.byte & (1 << i)) != 0;
        std::cout << (int)bit << " ";
        m_inputBuffer.push(bit);
    }
    std::cout << std::endl;
    return true;
}
bool Mapper::output(filter_io_t &data)
{
    if (m_tickCount == 1) {
        data = m_output;
        if (std::abs(data.toComplexDouble()) > 0.01) {
            std::cout << "mapper out: " << data.toComplexDouble() << std::endl;
        }
        return true;
    }
    return false;
}
void Mapper::tick(void)
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

symbol_t Mapper::getNextSymbol()
{
    symbol_t symbol = 0;
    for (size_t i = 0; i < m_bitsPerSymbol; i++) {
        symbol_t nextBit = !!m_inputBuffer.front(); //lsb first
        m_inputBuffer.pop();
        symbol |= (nextBit << i);
    }
    return symbol;
}

/* Static constellation generators */

constellation_map_t Mapper::getBPSKConstellations()
{
    constellation_map_t constellations;
    constellations[0b0] = constellation_t(32, 2);
    constellations[0b1] = constellation_t(32, 2);

    constellations[0b0].set(-1.0, 0.0);
    constellations[0b1].set(1.0, 0.0);

    return constellations;
}
constellation_map_t Mapper::getQPSKConstellations()
{
    constellation_map_t constellations;
    constellations[0b00] = constellation_t(32, 2);
    constellations[0b01] = constellation_t(32, 2);
    constellations[0b11] = constellation_t(32, 2);
    constellations[0b10] = constellation_t(32, 2);

    constellations[0b00].set(-M_SQRT1_2, -M_SQRT1_2);
    constellations[0b01].set(-M_SQRT1_2,  M_SQRT1_2);
    constellations[0b11].set( M_SQRT1_2,  M_SQRT1_2);
    constellations[0b10].set( M_SQRT1_2, -M_SQRT1_2);

    return constellations;
}
constellation_map_t Mapper::get8PSKConstellations()
{
    constellation_map_t constellations;
    for (size_t i = 0; i < 8; i++) {
        constellations[i] = constellation_t(32, 2);
    }
    constellations[0b000].set(-M_SQRT1_2, -M_SQRT1_2);
    constellations[0b001].set(-1.0,  0.0);
    constellations[0b011].set(-M_SQRT1_2,  M_SQRT1_2);
    constellations[0b010].set( 0.0,  1.0);
    constellations[0b110].set( M_SQRT1_2,  M_SQRT1_2);
    constellations[0b111].set( 1.0,  0.0);
    constellations[0b101].set( M_SQRT1_2, -M_SQRT1_2);
    constellations[0b100].set( 0.0, -1.0);

    return constellations;
}
constellation_map_t Mapper::getQAM16Constellations()
{
    //https://upload.wikimedia.org/wikipedia/commons/9/90/QAM16_Demonstration.gif
    constellation_map_t constellations;
    //Top Right
    for (size_t i = 0; i < 16; i++) {
            constellations[i] = constellation_t(32, 2);
    }
    constellations[0b0000].set( M_SQRT1_2/3,  M_SQRT1_2/3);
    constellations[0b0001].set( M_SQRT1_2  ,  M_SQRT1_2/3);
    constellations[0b0010].set( M_SQRT1_2/3,  M_SQRT1_2  );
    constellations[0b0011].set( M_SQRT1_2  ,  M_SQRT1_2  );

    //Bottom Right
    constellations[0b0100].set( M_SQRT1_2/3, -M_SQRT1_2/3);
    constellations[0b0101].set( M_SQRT1_2/3, -M_SQRT1_2  );
    constellations[0b0110].set( M_SQRT1_2,   -M_SQRT1_2/3);
    constellations[0b0111].set( M_SQRT1_2  , -M_SQRT1_2  );

    //Top Left
    constellations[0b1000].set(-M_SQRT1_2/3,  M_SQRT1_2/3);
    constellations[0b1001].set(-M_SQRT1_2/3,  M_SQRT1_2  );
    constellations[0b1010].set(-M_SQRT1_2,    M_SQRT1_2/3);
    constellations[0b1011].set(-M_SQRT1_2  ,  M_SQRT1_2  );

    //Bottom Left
    constellations[0b1100].set(-M_SQRT1_2/3, -M_SQRT1_2/3);
    constellations[0b1101].set(-M_SQRT1_2,   -M_SQRT1_2/3);
    constellations[0b1110].set(-M_SQRT1_2/3, -M_SQRT1_2  );
    constellations[0b1111].set(-M_SQRT1_2  , -M_SQRT1_2  );

    return constellations;
}
