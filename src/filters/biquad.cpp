#include <filters/biquad.hpp>
#include <cassert>

Biquad::Biquad() :
    FilterChainElement("Biquad"),
    m_x(3, FixedComplex16(0.0, 0.0)),
    m_y(3, FixedComplex16(0.0, 0.0)),
    m_b(3, FixedComplex16(0.0, 0.0)),
    m_a(2, FixedComplex16(0.0, 0.0)),
    m_Gb(0.0),
    m_Ga(0.0),
    m_newInput(false)
{

}

void Biquad::init(const FixedComplex16 &b0,
        const FixedComplex16 &b1,
        const FixedComplex16 &b2,
        const FixedComplex16 &a1,
        const FixedComplex16 &a2,
        const FixedComplex16 &Gb,
        const FixedComplex16 &Ga)
{

}

bool Biquad::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_FIXED_COMPLEX_32_NEW);
    //shift inputs to make room for new sample
    size_t size = m_x.size();
    for (size_t i = 0; i < size - 1; i++) {
        m_x[size - i - 1] = m_x[size - i - 2];
    }
    m_x[0] = data.fcn32;
    m_newInput = true;
    return true;
}

bool Biquad::output(filter_io_t &data)
{
    if (m_newInput) {
        data = m_y[0];
        m_newInput = false;
        return true;
    }
    return false;
}

void Biquad::tick(void)
{
    if (!m_newInput) {
        return;
    }

    //shift outputs to make room for new sample
    size_t size = m_y.size();
    for (size_t i = 0; i < size - 1; i++) {
        m_y[size - i - 1] = m_y[size - i - 2];
    }
    //m_y[0] = (b0 * m_x[0]) + (b1 * m_x[1]) + (b2 * m_x[2]) //Feedforward terms
    //        + (a1 * m_y[1]) + (a2 * m_y[2]); //Feedback terms
}
