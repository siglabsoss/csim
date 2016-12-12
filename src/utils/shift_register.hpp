#include <vector>
#include <cstdlib>
#include <bitset>

#pragma once

template <size_t regSize>
class ShiftRegister
{
public:
    ShiftRegister();
    bool shiftRight(bool bitIn);
    bool shiftLeft(bool bitIn);

    bool operator[](size_t idx) const;
    ShiftRegister &operator=(const std::bitset<regSize> &newVal);
    ShiftRegister &operator=(unsigned newVal);
private:
    std::bitset<regSize> m_bits;
};

//This is one way of keeping the 'templatized' implementation in a separate file while avoiding linker problems
//with particular specializations of the class
#include "shift_register.cpp"
