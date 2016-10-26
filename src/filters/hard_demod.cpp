#include <filters/hard_demod.hpp>
#include <cassert>

HardDemod::HardDemod(Mapper::constellation_set_t scheme, double theta) :
    m_bits(),
    m_value(),
    m_inputValid(false),
    m_theta(theta),
    m_constellations(),
    m_bitsPerSymbol(0)
{
    switch(scheme) {
        case Mapper::CONST_SET_BPSK:
            m_constellations = Mapper::getBPSKConstellations();
            m_bitsPerSymbol = 1;
            break;
        case Mapper::CONST_SET_QPSK:
            m_constellations = Mapper::getQPSKConstellations();
            m_bitsPerSymbol = 2;
            break;
        case Mapper::CONST_SET_8PSK:
            m_constellations = Mapper::get8PSKConstellations();
            m_bitsPerSymbol = 3;
            break;
        case Mapper::CONST_SET_QAM16:
            m_constellations = Mapper::getQAM16Constellations();
            m_bitsPerSymbol = 4;
            break;
        case Mapper::CONST_SET_NULL:
        default:
            break;
    }
}

HardDemod::~HardDemod()
{
}

bool HardDemod::input(const filter_io_t &data)
{
    m_value = data;
    m_inputValid = true;
    return true;
}

bool HardDemod::output(filter_io_t &data)
{
    bool haveByte = dequeueByte(data.byte);

    if (haveByte) {
        data.type = IO_TYPE_BYTE;
    }
    return haveByte;
}

void HardDemod::tick(void)
{
    if (!m_inputValid) {
        return;
    }
    m_inputValid = false;
    ComplexDouble value = m_value.toComplexDouble();
    if (abs(value) < 0.1) { //XXX define threshold
        return;
    }
    symbol_t symbol;
    double minDistance = 1e100;
    for (auto it = m_constellations.begin(); it != m_constellations.end(); it++) {
        double distance = abs(it->second.toComplexDouble() - value);
        if (distance < minDistance) {
            minDistance = distance;
            symbol = it->first;
        }
    }
    queueSymbol(symbol);
}

void HardDemod::queueSymbol(symbol_t symbol)
{
    for (size_t i = 0; i < m_bitsPerSymbol; i++) {
        bool value = symbol & (1 << i);
        m_bits.push(value);
    }
}

bool HardDemod::dequeueByte(uint8_t &byte)
{
    if (m_bits.size() < 8) {
        return false;
    }
    byte = 0;
    for (size_t i = 0; i < 8; i++) {
        bool nextBit = !!m_bits.front();
        m_bits.pop();
        byte |= (nextBit << i);
    }
    return true;
}

double HardDemod::angleDiff(double a, double b)
{
    double diff = fabs(a - b);
    if (diff > M_PI) {
        diff = 2 * M_PI - diff;
    }
    return diff;
}
