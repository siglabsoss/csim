#include <utils/shift_register.hpp>

ShiftRegister::ShiftRegister(size_t regSize) :
        m_bits(regSize)
{
}

bool ShiftRegister::shiftRight(bool bitIn)
{
    bool bitOut = m_bits[m_bits.size() - 1];
    for (size_t i = m_bits.size() - 1; i > 0; i--) {
        m_bits[i] = m_bits[i - 1];
    }
    m_bits[0] = bitIn;
    return bitOut;
}

bool ShiftRegister::shiftLeft(bool bitIn)
{
    bool bitOut = m_bits[0];
    for (size_t i = 0; i < m_bits.size()-1; i++) {
        m_bits[i] = m_bits[i + 1];
    }
    m_bits[m_bits.size()-1] = bitIn;
    return bitOut;
}

bool ShiftRegister::operator[](size_t idx) const
{
    return m_bits[idx];
}

ShiftRegister & ShiftRegister::operator=(const std::vector<bool> &newVal)
{
    m_bits = newVal;
    return *this;
}
