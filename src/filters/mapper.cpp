#include <filters/mapper.hpp>
#include <filters/fft.hpp>

#include <core/parameters.hpp>

#include <cassert>

Mapper::Mapper(unsigned int ticksPerSymbol, MCS mcs) :
    FilterChainElement("Mapper"),
    m_constellations(),
    m_bitsPerSymbol(0),
    m_inputBuffer(),
    m_output(),
    m_scrambler(0b1111111),
    m_tickCount(0),
    m_ticksPerSymbol(ticksPerSymbol),
    m_gotFirstSymbol(false)
{
    MCS::modulation_t scheme = mcs.getModulation();
    m_tickCount = m_ticksPerSymbol; //set to trigger an output update on first iteration
    assert(scheme != MCS::MOD_QAM64); //not implemented
    m_bitsPerSymbol = mcs.getNumBitsPerSubcarrier();
    switch(scheme) {
        case MCS::MOD_BPSK:
            m_constellations = getBPSKConstellations();
            break;
        case MCS::MOD_QPSK:
            m_constellations = getQPSKConstellations();
            break;
        case MCS::MOD_8PSK:
            m_constellations = get8PSKConstellations();
            break;
        case MCS::MOD_QAM16:
            m_constellations = getQAM16Constellations();
            break;
        case MCS::MOD_QAM64:
        case MCS::MOD_NULL:
        default:
            break;
    }
}

bool Mapper::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_BIT);
    //make sure we have enough space left to store the next symbol
    if (INPUT_BUFFER_BITS_MAX - m_inputBuffer.size() < 1) {
        return false;
    }
    //queuing up least significant bit first
    m_inputBuffer.push(data.bit);
    if (!m_gotFirstSymbol && (m_inputBuffer.size() >= m_bitsPerSymbol)) {
        m_gotFirstSymbol = true;
    }
    return true;
}

bool Mapper::output(filter_io_t &data)
{
    if (m_gotFirstSymbol && m_tickCount == 1) {
        data = m_output;
//        if (std::abs(data.toComplexDouble()) > 0.01) {
//            std::cout << "mapper out: " << data.toComplexDouble() << std::endl;
//        }
        return true;
    }
    return false;
}

void Mapper::tick(void)
{
    if (m_tickCount >= m_ticksPerSymbol) {
        symbol_t symbol = NULL_SYMBOL;
        if (m_inputBuffer.size() >= m_bitsPerSymbol) { //we have real input to modulate
            symbol   = getNextSymbol();
        } else {
            //XXX For now we will assume our input buffer will never by starved after we've started getting symbols.
            //Later we will have to decide how this block will handle this case
            assert(!m_gotFirstSymbol);
            symbol   = generateRandomSymbol();
        }
        m_output = m_constellations.at(symbol);
        m_tickCount = 0;
    }
    m_tickCount++;
}

symbol_t Mapper::getNextSymbol()
{
    symbol_t symbol = 0;
    for (size_t i = 0; i < m_bitsPerSymbol; i++) {
        symbol_t nextBit = static_cast<bool>(!!m_inputBuffer.front()); //first in, first out
        m_inputBuffer.pop();
        symbol |= (nextBit << i);
    }
    return symbol;
}

symbol_t Mapper::generateRandomSymbol()
{
    symbol_t symbol = 0;
    for (size_t i = 0; i < m_bitsPerSymbol; i++) {
        symbol_t nextBit = m_scrambler.scramble(0); //scramble zeros to get 'random' pattern
        symbol |= (nextBit << i);
    }
    return symbol;
}

/* Static constellation generators */

constellation_map_t Mapper::getBPSKConstellations()
{
    constellation_map_t constellations;
    constellations[0b0] = constellation_t(FFT_INPUT_FORMAT);
    constellations[0b1] = constellation_t(FFT_INPUT_FORMAT);

    constellations[0b0].set(-1.0, 0.0);
    constellations[0b1].set(1.0, 0.0);

    return constellations;
}

constellation_map_t Mapper::getQPSKConstellations()
{
    constellation_map_t constellations;
    constellations[0b00] = constellation_t(FFT_INPUT_FORMAT);
    constellations[0b01] = constellation_t(FFT_INPUT_FORMAT);
    constellations[0b11] = constellation_t(FFT_INPUT_FORMAT);
    constellations[0b10] = constellation_t(FFT_INPUT_FORMAT);

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
        constellations[i] = constellation_t(FFT_INPUT_FORMAT);
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
            constellations[i] = constellation_t(FFT_INPUT_FORMAT);
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

