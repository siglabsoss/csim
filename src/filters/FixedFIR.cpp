#include <filters/FixedFIR.hpp>


FixedFIR::FixedFIR(int N, FixedComplex<16>* tap) :
    m_taps(N),
    m_bench(N),
    m_output()
{
    for (int i = 0; i < N; i++) {
        m_taps[i] = tap[i];
    }
}

bool FixedFIR::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_FIXED_COMPLEX_16);
    FixedComplex<16> sample = data.fc;
    m_output = filter(sample);
    return true;
}
/**
 * output - provide an output sample to the caller.
 * @return false if no output sample is available.
 */
bool FixedFIR::output(filter_io_t &data)
{
    data.type = IO_TYPE_FIXED_COMPLEX_16;
    data.fc = m_output;
    return true;
}

void FixedFIR::tick()
{
}


void FixedFIR::reset()
{
    for (int i = 0; i < m_bench.size(); i++) {
        m_bench[i].real = 0;
        m_bench[i].imag = 0;
    }
}

FixedComplex<16> FixedFIR::filter(FixedComplex<16> &input)
{

    FixedComplex<32> sum;

    //m_bench[0] = input; //New data on bench
    m_bench.push_front(input);

    sum.real = 1 << 14;
    sum.imag = 1 << 14;
    for (int j = 0; (j < m_bench.size()); j++) {
        sum = sum + (m_bench[j].to_32() * m_taps[j].to_32());
    } //Accumulate

    sum = sum >> 15;
    return sum.to_16();

}

