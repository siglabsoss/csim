#pragma once

#include <utils/shift_register.hpp>

class Scrambler
{
public:
    Scrambler(const std::vector<bool> &initState);
    void scramble(std::vector<bool> &data);
    void reset(const std::vector<bool> &initState);
private:
    ShiftRegister m_reg;
};
