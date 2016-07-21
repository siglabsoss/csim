#include "cic.h"

using namespace std;

fixedcic::fixedcic(int R, int aregs, int bregs) :
        m_r(R),
        m_numBRegisters(bregs),
        m_numARegisters(aregs),
        m_a(aregs),
        m_b(bregs),
        m_samples(0)
{}

void fixedcic::cic(FixedComplex<16> &input)
{

    FixedComplex<32> temp; //Storage for integrate output
    temp = integrate(input); //Calculate filtered data
    if (!(this->downsample())) //If not downsampled
        m_output = ((this->comb(temp))).to_16(); //converts final value of comb to 16 bits.

}

void fixedcic::reset()
{
    m_samples = 0; //reset sample count

    for (int i = 0; i < m_numBRegisters; i++) {
         m_b[i].real = 0;
         m_b[i].imag = 0;

     } //Initialize registers

     for (int i = 0; i < m_numARegisters; i++) {
         m_a[i].real = 0;
         m_a[i].imag = 0;
     } //Initialize registers
}

FixedComplex<32> fixedcic::integrate(FixedComplex<16> current)
{
    FixedComplex<32> CenterTap;

    CenterTap = current.to_32();

    for (int i = 0; i < m_numBRegisters; i++) {
        CenterTap = CenterTap + m_b[i].to_32(); //Accumulate for each b register
        m_b[i] = CenterTap;
    }

    return CenterTap;
} //Performs filtering

FixedComplex<32> fixedcic::comb(FixedComplex<32> current)
{
    FixedComplex<32> final;
    FixedComplex<32> temp; //storage for swap

    final = current.to_32();

    for (int i = 0; i < m_numARegisters; i++) {
        temp = final;
        final = final - m_a[i].to_32(); //Accumulate for each b register
        m_a[i] = temp;
    }

    return final;
} //Performs

bool fixedcic::downsample()
{
    if ((m_samples % m_r) == 0) {
        m_samples++;
        return false;
    } else //otherwise do not comb
    {
        m_samples++;
        return true;
    }
}


