#include <types/fixedcomplex.hpp>

SLFixComplex::SLFixComplex() :
    m_real(),
    m_imag()
{

}

SLFixComplex::SLFixComplex(size_t wordLength, size_t intLength) :
    m_real(wordLength, intLength),
    m_imag(wordLength, intLength)
{

}

SLFixComplex SLFixComplex::operator+(const SLFixComplex &rhs)
{
    assert(m_real.m_wl != 0);
    SLFixComplex result(m_real.m_wl, m_real.m_wl - m_real.m_fl);
    result.m_real = this->m_real + rhs.m_real;
    result.m_imag = this->m_imag + rhs.m_imag;

    return result;
}

SLFixComplex SLFixComplex::operator-(const SLFixComplex &rhs)
{
    SLFixComplex result(m_real.m_wl, m_real.m_wl - m_real.m_fl);
    result.m_real = this->m_real - rhs.m_real;
    result.m_imag = this->m_imag - rhs.m_imag;

    return result;
}

SLFixComplex SLFixComplex::operator*(const SLFixComplex &rhs)
{
    SLFixComplex result(m_real.m_wl + rhs.real().m_wl, m_real.m_wl + rhs.real().m_wl - (m_real.m_fl + rhs.real().m_fl));

    result.m_real = (this->m_real * rhs.m_real) - (this->m_imag * rhs.m_imag);
    result.m_imag = (this->m_real * rhs.m_imag) + (this->m_imag * rhs.m_real);

    return result;
}

SLFixComplex SLFixComplex::operator*(const SLFixPoint &rhs)
{
    SLFixComplex result(m_real.m_wl + rhs.m_wl, m_real.m_wl + rhs.m_wl - (m_real.m_fl + rhs.m_fl));

    result.m_real = this->m_real * rhs;
    result.m_imag = this->m_imag * rhs;

    return result;
}

SLFixComplex SLFixComplex::operator/(const SLFixComplex &rhs)
{
    //XXX implement
    SLFixComplex result(0, 0);
    return result;
}

SLFixComplex &SLFixComplex::operator=(const SLFixComplex &rhs)
{
    this->m_real = rhs.m_real;
    this->m_imag = rhs.m_imag;
    return *this;
}

SLFixComplex &SLFixComplex::operator=(double val)
{
    this->m_real = val;
    this->m_imag = val;
    return *this;
}

SLFixComplex &SLFixComplex::operator<<(size_t shift)
{
    this->m_real = this->m_real << shift;
    this->m_imag = this->m_imag << shift;
    return *this;
}

SLFixComplex &SLFixComplex::operator>>(size_t shift)
{
    this->m_real = this->m_real >> shift;
    this->m_imag = this->m_imag >> shift;
    return *this;
}

void SLFixComplex::setFormat(size_t wordLength, size_t intLength)
{
    m_real.setFormat(wordLength, intLength);
    m_imag.setFormat(wordLength, intLength);
}

SLFixPoint  SLFixComplex::real() const
{
    return m_real;
}

void        SLFixComplex::real(const SLFixPoint &val)
{
    m_real = val;
}

void        SLFixComplex::real(double val)
{
    m_real = val;
}

SLFixPoint  SLFixComplex::imag() const
{
    return m_imag;
}

void        SLFixComplex::imag(const SLFixPoint &val)
{
    m_imag = val;
}

void        SLFixComplex::imag(double val)
{
    m_imag = val;
}

std::ostream& operator<<(std::ostream& os, const SLFixComplex& obj)
{
    os << "(" << obj.real() << ", " << obj.imag() << ")";
    return os;
}
