#include <types/fixedcomplex.hpp>
#include <cassert>

SLFixComplex::SLFixComplex() :
    m_real(),
    m_imag()
{}

SLFixComplex::SLFixComplex(size_t                      wordLength,
                           ssize_t                     intLength,
                           SLFixPoint::quant_mode_t    quantMode,
                           SLFixPoint::overflow_mode_t overflowMode) :
    m_real(wordLength, intLength, quantMode, overflowMode),
    m_imag(wordLength, intLength, quantMode, overflowMode)
{}

SLFixComplex SLFixComplex::operator+(const SLFixComplex& rhs) const
{
    assert(m_real.wl() != 0);
    SLFixComplex result;
    SLFixPoint   newReal = this->m_real + rhs.m_real;
    SLFixPoint   newImag = this->m_imag + rhs.m_imag;
    result.m_real.setFormat(newReal.getFormat());
    result.m_real = newReal;
    result.m_imag.setFormat(newImag.getFormat());
    result.m_imag = newImag;

    return result;
}

SLFixComplex SLFixComplex::operator-(const SLFixComplex& rhs) const
{
    assert(m_real.wl() != 0);
    SLFixComplex result;
    SLFixPoint   newReal = this->m_real - rhs.m_real;
    SLFixPoint   newImag = this->m_imag - rhs.m_imag;
    result.m_real.setFormat(newReal.getFormat());
    result.m_real = newReal;
    result.m_imag.setFormat(newImag.getFormat());
    result.m_imag = newImag;

    return result;
}

SLFixComplex& SLFixComplex::operator+=(const SLFixComplex& rhs)
{
    this->m_real += rhs.m_real;
    this->m_imag += rhs.m_imag;
    return *this;
}

SLFixComplex& SLFixComplex::operator-=(const SLFixComplex& rhs)
{
    this->m_real -= rhs.m_real;
    this->m_imag -= rhs.m_imag;
    return *this;
}

SLFixComplex SLFixComplex::operator*(const SLFixComplex& rhs) const
{
    SLFixComplex result;

    SLFixPoint newReal = (this->m_real * rhs.m_real) - (this->m_imag * rhs.m_imag);
    SLFixPoint newImag = (this->m_real * rhs.m_imag) + (this->m_imag * rhs.m_real);

    result.m_real.setFormat(newReal.getFormat());
    result.m_real = newReal;
    result.m_imag.setFormat(newImag.getFormat());
    result.m_imag = newImag;

    return result;
}

SLFixComplex SLFixComplex::operator*(const SLFixPoint& rhs) const
{
    SLFixComplex result;

    SLFixPoint newReal = this->m_real * rhs;
    SLFixPoint newImag = this->m_imag * rhs;

    result.m_real.setFormat(newReal.getFormat());
    result.m_real = newReal;
    result.m_imag.setFormat(newImag.getFormat());
    result.m_imag = newImag;

    return result;
}

SLFixComplex SLFixComplex::operator/(const SLFixComplex& rhs) const
{
    // XXX implement
    SLFixComplex result(0,
                        0,
                        SLFixPoint::QUANT_TRUNCATE,
                        SLFixPoint::OVERFLOW_WRAP_AROUND);

    return result;
}

SLFixComplex& SLFixComplex::operator=(const SLFixComplex& rhs)
{
    this->m_real = rhs.m_real;
    this->m_imag = rhs.m_imag;
    return *this;
}

SLFixComplex& SLFixComplex::operator=(const SLFixPoint& rhs)
{
    this->m_real = rhs;
    this->m_imag = 0.0;
    return *this;
}

SLFixComplex& SLFixComplex::operator=(ComplexDouble val)
{
    this->m_real = val.real();
    this->m_imag = val.imag();
    return *this;
}

SLFixComplex& SLFixComplex::operator<<(size_t shift)
{
    this->m_real = this->m_real << shift;
    this->m_imag = this->m_imag << shift;
    return *this;
}

SLFixComplex& SLFixComplex::operator>>(size_t shift)
{
    this->m_real = this->m_real >> shift;
    this->m_imag = this->m_imag >> shift;
    return *this;
}

SLFixComplex SLFixComplex::operator*()
{
    SLFixComplex result;

    result.m_real.setFormat(this->m_real.getFormat());
    result.m_real = m_real;

    result.m_imag.setFormat(this->m_imag.getFormat());
    result.m_imag = -(this->m_imag.to_double());

    return result;
}

bool SLFixComplex::operator==(const SLFixComplex& rhs) const
{
    return (m_real == rhs.m_real) && (m_imag == rhs.m_imag);
}

void SLFixComplex::shiftRadixRight(size_t shiftAmount)
{
    this->m_real.shiftRadixRight(shiftAmount);
    this->m_imag.shiftRadixRight(shiftAmount);
}

void SLFixComplex::shiftRadixLeft(size_t shiftAmount)
{
    this->m_real.shiftRadixLeft(shiftAmount);
    this->m_real.shiftRadixLeft(shiftAmount);
}

void SLFixComplex::setFormat(size_t                      wordLength,
                             ssize_t                     intLength,
                             SLFixPoint::quant_mode_t    quantMode,
                             SLFixPoint::overflow_mode_t overflowMode)
{
    m_real.setFormat(wordLength, intLength, quantMode, overflowMode);
    m_imag.setFormat(wordLength, intLength, quantMode, overflowMode);
}

void SLFixComplex::setFormat(const SLFixPoint::format_t& fmt)
{
    m_real.setFormat(fmt);
    m_imag.setFormat(fmt);
}

SLFixPoint::format_t SLFixComplex::getFormat() const
{
    return m_real.getFormat();
}

SLFixPoint SLFixComplex::real() const
{
    return m_real;
}

void SLFixComplex::real(const SLFixPoint& val)
{
    m_real = val;
}

void SLFixComplex::real(double val)
{
    m_real = val;
}

SLFixPoint SLFixComplex::imag() const
{
    return m_imag;
}

void SLFixComplex::imag(const SLFixPoint& val)
{
    m_imag = val;
}

void SLFixComplex::imag(double val)
{
    m_imag = val;
}

void SLFixComplex::set(double real, double imag)
{
    m_real = real;
    m_imag = imag;
}

ComplexDouble SLFixComplex::toComplexDouble() const
{
    return ComplexDouble(m_real.to_double(), m_imag.to_double());
}

size_t SLFixComplex::wl()  const
{
    // real/imag word lengths are equal
    return m_real.wl();
}

ssize_t SLFixComplex::iwl() const
{
    // real/imag int word lengths are equal
    return m_real.iwl();
}

bool SLFixComplex::isFormatSet() const
{
    return m_real.isFormatSet();
}

std::ostream& operator<<(std::ostream& os, const SLFixComplex& obj)
{
    os << "(" << obj.real() << ", " << obj.imag() << ")";
    return os;
}
