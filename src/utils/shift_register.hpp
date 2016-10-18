#include <vector>
#include <cstdlib>

#pragma once

class ShiftRegister
{
public:
    ShiftRegister(size_t regSize);
    bool shiftRight(bool bitIn);
    bool shiftLeft(bool bitIn);

    bool operator[](size_t idx) const;
    ShiftRegister &operator=(const std::vector<bool> &newVal);
private:
    std::vector<bool> m_bits;
};

