#include <utils/scrambler.hpp>
#include <cassert>
#include <iostream>

Scrambler::Scrambler(const std::bitset<SCRAMBLER_SHIFT_REGISTER_SIZE> &initState) :
    m_reg()
{
    reset(initState);
}

void Scrambler::scramble(std::vector<bool> &data)
{
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = scramble(data[i]);
    }
}

bool Scrambler::scramble(bool bit)
{
    bool shifterBit = (m_reg[3] ^ m_reg[6]); //XOR bit 4 and 7
    bit = (shifterBit ^ bit);
    (void)m_reg.shiftLeft(shifterBit); //Feedback result of first XOR into shift register
    return bit;
}

void Scrambler::reset(const std::bitset<SCRAMBLER_SHIFT_REGISTER_SIZE> &initState)
{
    assert(initState.size() == SCRAMBLER_SHIFT_REGISTER_SIZE);
    m_reg = initState;
}
