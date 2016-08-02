#include <filters/fixed_fir.hpp>


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

bool FixedFIR::output(filter_io_t &data)
{
    data = m_output;
    return true;
}

void FixedFIR::tick()
{
}


void FixedFIR::reset()
{
    for (int i = 0; i < m_bench.size(); i++) {
        m_bench[i].real(0);
        m_bench[i].imag(0);
    }
}

FixedComplex<16> FixedFIR::filter(FixedComplex<16> &input)
{

    FixedComplex<16> sum;

    m_bench.push_front(input);

    for (int j = 0; (j < m_bench.size()); j++) {
        sum = sum + (m_bench[j] * m_taps[j]);
    } //Accumulate

    return sum;
}

