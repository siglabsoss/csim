
#include <filters/mixer.hpp>

Mixer::~Mixer() {}

Mixer::Mixer(uint32_t ticksPerPeriod, bool upMix) :
    FilterChainElement(std::string("Mixer")),
    m_count(upMix ? 0 : ticksPerPeriod),
    m_ticksPerPeriod(ticksPerPeriod),
    m_inputValid(false),
    m_upMix(upMix)
{
}

bool Mixer::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_FIXED_COMPLEX || data.type == IO_TYPE_COMPLEX_DOUBLE);
    m_inputValid = true;
    m_input = data.toComplexDouble();
    return true;
}

bool Mixer::output(filter_io_t &data)
{
    m_inputValid = false;
    data = m_carrier * m_input;
    return true;
}

void Mixer::tick(void)
{
    double theta = (((2 * M_PI) / m_ticksPerPeriod) * m_count);

    m_carrier = ComplexDouble(cos(theta), sin(theta));

    if (m_upMix) {
        m_count++;
        if (m_count >= m_ticksPerPeriod) {
            m_count -= m_ticksPerPeriod;
        }
    } else {
        m_count--;
        if (m_count <= 0) {
            m_count += m_ticksPerPeriod;
        }
    }
}
