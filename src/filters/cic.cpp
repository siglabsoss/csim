#include <filters/cic.hpp>

using namespace std;

fixedcic::fixedcic(int R, int aregs, int bregs) :
    m_r(R),
    m_numBRegisters(bregs),
    m_numARegisters(aregs),
    m_a(aregs),
    m_b(bregs)
{
    m_samples = 0;
    goodOutput = false;
}


bool fixedcic::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_FIXED_COMPLEX_16_NEW);
    FixedComplex2<16, 1> sample = data.fcn;
    cic(sample);
    return true;
}
/**
 * output - provide an output sample to the caller.
 * @return false if no output sample is available.
 */
bool fixedcic::output(filter_io_t &data)
{
    if (goodOutput == true) {
        data = m_output;
        //XXX should goodOutput be reset here?
        return true;
    }//Valid data
    else {
        return false;
    }//Not valid data
}

void fixedcic::tick()
{

}

void fixedcic::cic(FixedComplex2<16, 1> &input)
{
    goodOutput = false;
    FixedComplex2<16, 1> temp; //Storage for integrate output
    temp = integrate(input); //Calculate filtered data
    if (!(this->downsample())) {//If not downsampled
        m_output = ((this->comb(temp))); //converts final value of comb to 16 bits.
        goodOutput= true;
    }
}

void fixedcic::reset()
{
    m_samples = 0; //reset sample count

    for (int i = 0; i < m_numBRegisters; i++) {
         m_b[i].real(0);
         m_b[i].imag(0);

     } //Initialize registers

     for (int i = 0; i < m_numARegisters; i++) {
         m_a[i].real(0);
         m_a[i].imag(0);
     } //Initialize registers
}

FixedComplex2<16, 1> fixedcic::integrate(FixedComplex2<16, 1> current)
{
    for (int i = 0; i < m_numBRegisters; i++) {
        current = current + m_b[i]; //Accumulate for each b register
        m_b[i] = current;
    }

    return current;
} //Performs filtering

FixedComplex2<16, 1> fixedcic::comb(FixedComplex2<16, 1> current)
{
    FixedComplex2<16, 1> final;
    FixedComplex2<16, 1> temp; //storage for swap

    final = current;

    for (int i = 0; i < m_numARegisters; i++) {
        temp = final;
        final = final - m_a[i]; //Accumulate for each b register
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


