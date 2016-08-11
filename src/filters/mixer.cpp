
#include <filters/mixer.hpp>

Mixer::~Mixer() {}

Mixer::Mixer(uint32_t fs, int32_t hz) :
    FilterChainElement(std::string("Mixer")),
    m_fs(fs),
    m_count(0),
    m_ticksPerPeriod(static_cast<int32_t>(fs)/hz)
{
    m_output.type = IO_TYPE_COMPLEX_DOUBLE;
}

bool Mixer::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_FIXED_COMPLEX_32_NEW);
    m_hold = data;
    return true;
}

bool Mixer::output(filter_io_t &data)
{
    data = m_output.fcn32 * m_hold.fcn32;
    //Convert to complex double
    data = ComplexDouble(data.fcn32.real(), data.fcn32.imag());
    return true;
}

void Mixer::tick(void)
{
    double theta = (((2 * M_PI) / m_ticksPerPeriod) * m_count);
    m_output.fcn32 = FixedComplex32(cos(theta), sin(theta));
    m_count++;
    if (m_count > m_ticksPerPeriod) {
        m_count -= m_ticksPerPeriod;
    }
}

