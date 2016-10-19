#include <cstdlib>
#include <iostream>

template <size_t regSize>
ShiftRegister<regSize>::ShiftRegister() :
        m_bits()
{
}

template <size_t regSize>
bool ShiftRegister<regSize>::shiftRight(bool bitIn)
{
    bool bitOut = m_bits[0];
    m_bits >>= 1;
    m_bits[regSize - 1] = bitIn;
    return bitOut;
}

template <size_t regSize>
bool ShiftRegister<regSize>::shiftLeft(bool bitIn)
{
    bool bitOut = m_bits[regSize - 1];
    m_bits <<= 1;
    m_bits[0] = bitIn;
    return bitOut;
}

template <size_t regSize>
bool ShiftRegister<regSize>::operator[](size_t idx) const
{
    return m_bits[idx];
}

template <size_t regSize>
ShiftRegister<regSize> & ShiftRegister<regSize>::operator=(const std::bitset<regSize> &newVal)
{
    m_bits = newVal;
    return *this;
}
