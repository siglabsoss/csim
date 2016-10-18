#include <utils/scrambler.hpp>
#include <cassert>
#include <iostream>

//802.11a scrambler requires 7-bit LFSR.
static constexpr size_t SCRAMBLER_SHIFT_REGISTER_SIZE = 7;

Scrambler::Scrambler(const std::vector<bool> &initState) :
    m_reg(SCRAMBLER_SHIFT_REGISTER_SIZE)
{
    reset(initState);
}

void Scrambler::scramble(std::vector<bool> &data)
{
    bool shifterBit;
    for (size_t i = 0; i < data.size(); i++) {
        shifterBit = (m_reg[3] + m_reg[6]) & 0x1; //XOR bit 4 and 7
        data[i] = (shifterBit + data[i]) & 0x1;  //XOR result of previous XOR and current data bit
        (void)m_reg.shiftRight(shifterBit);     //Feedback result of first XOR into shift register
    }
}

void Scrambler::reset(const std::vector<bool> &initState)
{
    assert(initState.size() == SCRAMBLER_SHIFT_REGISTER_SIZE);
    m_reg = initState;
}
