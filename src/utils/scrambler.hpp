#pragma once

#include <utils/shift_register.hpp>

//802.11a scrambler requires 7-bit LFSR.
static constexpr size_t SCRAMBLER_SHIFT_REGISTER_SIZE = 7;

class Scrambler
{
public:
    Scrambler(const std::bitset<SCRAMBLER_SHIFT_REGISTER_SIZE> &initState);
    void scramble(std::vector<bool> &data);
    bool scramble(bool bit);
    void reset(const std::bitset<SCRAMBLER_SHIFT_REGISTER_SIZE> &initState);
private:
    ShiftRegister<SCRAMBLER_SHIFT_REGISTER_SIZE> m_reg;
};
