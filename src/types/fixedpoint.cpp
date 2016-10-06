#include <types/fixedpoint.hpp>
#include <cassert>
#include <algorithm>

SLFixPoint::SLFixPoint() :
    m_value(0),
    m_wl(0),
    m_fl(0),
    m_formatSet(false),
    m_quantMode(QUANT_TRUNCATE),
    m_overflowMode(OVERFLOW_WRAP_AROUND)
{
}

SLFixPoint::SLFixPoint(size_t wordLength, ssize_t intLength) :
    m_value(0),
    m_wl(0),
    m_fl(0),
    m_formatSet(false),
    m_quantMode(QUANT_TRUNCATE),
    m_overflowMode(OVERFLOW_WRAP_AROUND)
{
    setFormat(wordLength, intLength);
}

SLFixPoint::SLFixPoint(const SLFixPoint &other) :
        m_value(other.m_value),
        m_wl(other.m_wl),
        m_fl(other.m_fl),
        m_formatSet(other.m_formatSet),
        m_quantMode(other.m_quantMode),
        m_overflowMode(other.m_overflowMode)
{

}

SLFixPoint::~SLFixPoint()
{

}

SLFixPoint SLFixPoint::addition(const SLFixPoint &rhs)
{
    size_t resultWordLength = std::max(this->m_wl, rhs.m_wl);
    ssize_t resultIntLength = resultWordLength - std::max(this->m_fl, rhs.m_fl);
    SLFixPoint tempRHS(resultWordLength, resultIntLength);
    SLFixPoint tempLHS(resultWordLength, resultIntLength);
    tempRHS = rhs;
    tempLHS = *this;

    assert(tempLHS.m_fl == tempRHS.m_fl);
    SLFixPoint result(resultWordLength, resultIntLength);
    result.m_value = tempLHS.m_value + tempRHS.m_value;
    result.maskAndSignExtend();
    return result;
}

SLFixPoint SLFixPoint::operator+(const SLFixPoint &rhs)
{
    return addition(rhs);
}

SLFixPoint &SLFixPoint::operator+=(const SLFixPoint &rhs)
{
    *this = addition(rhs);
    return *this;
}

SLFixPoint &SLFixPoint::operator-=(const SLFixPoint &rhs)
{
    SLFixPoint temp = rhs;
    temp.m_value = -temp.m_value;
    *this = addition(temp);
    return *this;
}

SLFixPoint SLFixPoint::operator-(const SLFixPoint &rhs)
{
    SLFixPoint temp = rhs;
    temp.m_value = -temp.m_value;
    return addition(temp);
}

SLFixPoint SLFixPoint::operator*(const SLFixPoint &rhs)
{
    assert(this->m_wl + rhs.m_wl <= sizeof(this->m_value)*8);
    ssize_t intWidth = (this->m_wl - this->m_fl) + (rhs.m_wl - rhs.m_fl);
    SLFixPoint result(this->m_wl + rhs.m_wl, intWidth);
    result.m_value = (this->m_value * rhs.m_value);
    result.maskAndSignExtend();
    return result;
}

SLFixPoint SLFixPoint::operator/(const SLFixPoint &rhs)
{
    //XXX TODO
    return *this;
}

SLFixPoint &SLFixPoint::operator=(const SLFixPoint &rhs)
{
    if (!m_formatSet) {
        setFormat(rhs);
    }
    ssize_t fracDiff = this->m_fl - rhs.m_fl;
    //Shift the value to align the fractional bits
    if (fracDiff >= 0) {
        this->m_value = (rhs.m_value << fracDiff);
    } else { //here we are losing precision
        fracDiff = -fracDiff;
        switch (m_quantMode) {
            case QUANT_RND_HALF_UP:
            {
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
                break;
            }
            case QUANT_TRUNCATE:
            default:
                this->m_value = (rhs.m_value >> fracDiff);
                break;
        }
    }
    maskAndSignExtend();
    return *this;
}

SLFixPoint &SLFixPoint::operator=(double val)
{
    assert(m_formatSet);
    this->m_value = static_cast<long long>(val * (1ull << m_fl));
    maskAndSignExtend();
    return *this;
}

SLFixPoint &SLFixPoint::operator<<(size_t shift)
{
    this->m_value <<= shift;
    this->m_fl += shift;
    return *this;
}

SLFixPoint &SLFixPoint::operator>>(size_t shift)
{
    this->m_value >>= shift;
    this->m_fl -= shift;
    return *this;
}

void SLFixPoint::shiftRadixRight(size_t shiftAmount)
{
    this->m_fl += shiftAmount;
}

void SLFixPoint::shiftRadixLeft(size_t shiftAmount)
{
    this->m_fl -= shiftAmount;
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
    double scale = 1.0 / static_cast<double>(1ull << this->m_fl);
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
//    if (this->m_value < 0) {
//        assert((~mask & -this->m_value) == 0);
//    } else {
//        assert((~mask & this->m_value) == 0);
//    }

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
    m_formatSet = true;
}

void SLFixPoint::setFormat(size_t wordLength, ssize_t intLength, quant_mode_t quantMode, overflow_mode_t overflowMode)
{
    m_quantMode = quantMode;
    m_overflowMode = overflowMode;
    setFormat(wordLength, intLength);
}

void SLFixPoint::setFormat(const SLFixPoint &other)
{
    setFormat(other.m_wl, other.m_fl, other.m_quantMode, other.m_overflowMode);
}

std::ostream& operator<<(std::ostream& os, const SLFixPoint& obj)
{
    os << obj.to_double();
    return os;
}
