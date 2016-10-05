#include <types/fixedpoint.hpp>
#include <cassert>

SLFixPoint::SLFixPoint() :
    m_value(0),
    m_wl(0),
    m_fl(0)
{
}

SLFixPoint::SLFixPoint(size_t wordLength, ssize_t intLength) :
    m_value(0),
    m_wl(0),
    m_fl(0)
{
    setFormat(wordLength, intLength);
}

SLFixPoint::SLFixPoint(const SLFixPoint &other) :
        m_value(other.m_value),
        m_wl(other.m_wl),
        m_fl(other.m_fl)
{

}

SLFixPoint::~SLFixPoint()
{

}

//SLFixPoint::SLFixPoint(double val) :
//        m_value(0),
//        m_wl(0),
//        m_fl(0)
//{
//
//}

SLFixPoint SLFixPoint::operator+(const SLFixPoint &rhs)
{
    assert(this->m_fl == rhs.m_fl);
    SLFixPoint result = *this;
    result.m_value = this->m_value + rhs.m_value;
    maskAndSignExtend();
    return result;
}

SLFixPoint SLFixPoint::operator-(const SLFixPoint &rhs)
{
    assert(this->m_fl == rhs.m_fl);
    SLFixPoint result = *this;
    result.m_value = this->m_value - rhs.m_value;
    maskAndSignExtend();
    return result;
}

SLFixPoint SLFixPoint::operator*(const SLFixPoint &rhs)
{
    assert(this->m_wl + rhs.m_wl <= sizeof(this->m_value)*8);
    SLFixPoint result(this->m_wl + rhs.m_wl, this->m_wl + rhs.m_wl - (this->m_fl + rhs.m_fl));
    result.m_value = (this->m_value * rhs.m_value);
    maskAndSignExtend();
    return result;
}

SLFixPoint SLFixPoint::operator/(const SLFixPoint &rhs)
{
    //XXX TODO
    return *this;
}

SLFixPoint &SLFixPoint::operator=(const SLFixPoint &rhs)
{
    ssize_t fracDiff = this->m_fl - rhs.m_fl;
    //Shift the value to align the fractional bits
    if (fracDiff >= 0) {
        this->m_value = (rhs.m_value << fracDiff);
    } else {
        //XXX precision loss! implement rounding
        fracDiff = -fracDiff;
        if (true) { //XXX make rounding an option
            //shift one bit short of the final amount so that we can take the last bit for rounding
            this->m_value = (rhs.m_value >> (fracDiff - 1));
            int roundBit = this->m_value & 0x01;
            this->m_value >>= 1; //shift that last bit off
            this->m_value += roundBit;
//            if (roundBit == 1) {
//                std::cout << "ROUNDING UP" << std::endl;
//            } else {
//                std::cout << "ROUNDING DOWN" << std::endl;
//            }
        } else {
            this->m_value = (rhs.m_value >> fracDiff);
        }
    }
    maskAndSignExtend();
    return *this;
}

SLFixPoint &SLFixPoint::operator=(double val)
{
    this->m_value = static_cast<long long>(val * (1 << m_fl));
    maskAndSignExtend();
    return *this;
}

SLFixPoint &SLFixPoint::operator<<(size_t shift)
{
    assert (shift >= 0);
    this->m_value <<= shift;
    this->m_fl += shift;
    this->m_wl += shift;
    return *this;
}

SLFixPoint &SLFixPoint::operator>>(size_t shift)
{
    assert (shift >= 0);
    this->m_value >>= shift;
    this->m_value = m_value;
    this->m_fl -= shift;
    this->m_wl -= shift;
    return *this;
}

uint64_t SLFixPoint::to_uint64() const
{
    return static_cast<uint64_t>(getMaskedValue());
}

int64_t SLFixPoint::to_int64() const
{
    return static_cast<int64_t>(this->m_value);
}

double SLFixPoint::to_double() const
{
    double scale = 1.0 / static_cast<double>(1 << this->m_fl);
    return static_cast<double>(this->m_value) * scale;
}

size_t SLFixPoint::wl() const
{
    return m_wl;
}

ssize_t SLFixPoint::iwl() const
{
    return static_cast<ssize_t>(m_wl - m_fl);
}

void SLFixPoint::extendSign()
{
    size_t spareBits = sizeof(m_value) * 8 - m_wl;
    long long val = m_value;
    val = (val << spareBits) >> spareBits;
    m_value = val;
}

long long SLFixPoint::getMaskedValue() const
{
    //XXX check for overflow, saturate, etc
    unsigned long long mask = ~0;
    mask >>= sizeof(this->m_value) * 8 - this->m_wl;
    if (this->m_value < 0) {
        assert((~mask & -this->m_value) == 0);
    } else {
        assert((~mask & this->m_value) == 0);
    }

    return (this->m_value & mask);
}

void SLFixPoint::maskAndSignExtend()
{
    this->m_value = getMaskedValue();
    extendSign();
}

void SLFixPoint::setFormat(size_t wordLength, ssize_t intLength)
{
    assert(static_cast<ssize_t>(wordLength) >= intLength);
    assert(wordLength <= sizeof(m_value)*8);
    //intLength can be negative to represent small values with
    //higher precision. thus m_fl can be greater than m_wl
    m_wl = wordLength;
    m_fl = wordLength - intLength;
}

std::ostream& operator<<(std::ostream& os, const SLFixPoint& obj)
{
    os << obj.to_double();
    return os;
}
