/*
 * fixediir.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: ubuntu
 */
/*
 *
 * NOTE: First a coefficient must be 1
 *
 */
#include <filters/fixediir.hpp>

void fixediir::reset()
{
    for (int i = 0; i < m_numXRegisters; i++) {
        m_x[i].real = 0;
        m_x[i].imag = 0;

    } //Initialize registers

    for (int i = 0; i < m_numYRegisters; i++) {
        m_y[i].real = 0;
        m_y[i].imag = 0;
    } //Initialize registers

}
bool fixediir::input(const filter_io_t &data)
{

    assert(data.type == IO_TYPE_FIXED_COMPLEX_16);
    FixedComplex<16> sample = data.fc;
    iir(sample);
    return true;
}
/**
 * output - provide an output sample to the caller.
 * @return false if no output sample is available.
 */
bool fixediir::output(filter_io_t &data)
{
    data.type = IO_TYPE_FIXED_COMPLEX_16;
    data.fc = m_output;
    return true;
}

void fixediir::tick()
{

}

fixediir::fixediir(int registerXSize, int registerYSize,
        FixedComplex<16>* aCoeffs, FixedComplex<16>* bCoeffs) :
                m_numXRegisters(registerXSize),
                m_numYRegisters(registerYSize),
                m_a(m_numYRegisters),
                m_b(m_numXRegisters),
                m_x(m_numXRegisters),
                m_y(m_numYRegisters)
{

    for (int i = 0; i < m_numYRegisters; i++)
        m_a[i] = aCoeffs[i]; //Gets coefficient data for right side

    for (int i = 0; i < m_numXRegisters; i++)
        m_b[i] = bCoeffs[i]; //Gets coefficient data for left side

} //Constructs filter based on number of registers on each side and the values of those registers

void fixediir::iir(FixedComplex<16> &input)
{

    FixedComplex<32> currenty, centerTap; //32 bits due to multiplication of 2 16 bits
    centerTap = (input.to_32() * m_b[0].to_32()); // x[0] * b[0]

    for (int i = 1; i < m_numXRegisters; i++)
        centerTap = centerTap + (m_x[i].to_32() * m_b[i].to_32()); //Accumulate for each x register

    centerTap = centerTap >> 15; //Reset to 16 bits

    currenty = (centerTap << 15); // Coefficient of a[0] = 1
    for (int i = 1; i < m_numYRegisters; i++) {
        currenty = (currenty) - (m_a[i].to_32() * m_y[i].to_32()); //Accumulate

    }

    currenty = currenty >> 15; //Reset to 16 bits

    for (int i = m_numXRegisters - 1; i > 1; i--)
        m_x[i] = m_x[i - 1]; //Moves everything up one register

    for (int i = m_numYRegisters - 1; i > 1; i--)
        m_y[i] = m_y[i - 1]; //Moves everything up one register

    m_x[1] = input; //Moves x[0] to x[1]
    m_y[1] = currenty.to_16(); //Moves y[0] to y[1]

     m_output = currenty.to_16(); //Calculate filtered data
} //Filters input data and places filtered data in output. Takes in input values, output array, and number of inputs.


