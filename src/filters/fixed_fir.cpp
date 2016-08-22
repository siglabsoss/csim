#include <filters/fixed_fir.hpp>

FixedFIR::FixedFIR(std::vector<FixedComplex16> tap) :
	FilterChainElement("FixedFIR"),
    m_taps(tap.size()),
    m_bench(tap.size()),
    m_output()
{
    for (int i = 0; i < tap.size(); i++) {
        m_taps[i] = tap[i];
    }
}

bool FixedFIR::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_FIXED_COMPLEX_16_NEW);
    FixedComplex16 sample = data.fcn;


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

FixedComplex16 FixedFIR::filter(FixedComplex16 &input)
{

    FixedComplex16 sum;

    m_bench.push_front(input);

    for (int j = 0; (j < m_bench.size()); j++) {
        sum = sum + (m_bench[j] * m_taps[j]);
    } //Accumulate

    return sum;
}

