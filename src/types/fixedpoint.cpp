#include <types/fixedpoint.hpp>
#include <cassert>
#include <algorithm>

bool     SLFixPoint::throwOnOverflow = false;
size_t   SLFixPoint::overflowCount = 0;
size_t   SLFixPoint::underflowCount = 0;
size_t   SLFixPoint::roundUpCount = 0;
size_t   SLFixPoint::roundDownCount = 0;
size_t   SLFixPoint::precisionLossCount = 0;

SLFixPoint::SLFixPoint() :
    m_value(0),
    m_wl(0),
    m_fl(0),
    m_formatSet(false),
    m_quantMode(DEFAULT_QUANT_MODE),
    m_overflowMode(DEFAULT_OVERFLOW_MODE)
{
}

SLFixPoint::SLFixPoint(size_t wordLength, ssize_t intLength) :
    m_value(0),
    m_wl(0),
    m_fl(0),
    m_formatSet(false),
    m_quantMode(DEFAULT_QUANT_MODE),
    m_overflowMode(DEFAULT_OVERFLOW_MODE)
{
    setFormat(wordLength, intLength);
}

SLFixPoint::SLFixPoint(size_t wordLength, ssize_t intLength, quant_mode_t quantMode, overflow_mode_t overflowMode) :
    m_value(0),
    m_wl(0),
    m_fl(0),
    m_formatSet(false),
    m_quantMode(DEFAULT_QUANT_MODE),
    m_overflowMode(DEFAULT_OVERFLOW_MODE)
{
    setFormat(wordLength, intLength, quantMode, overflowMode);
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

SLFixPoint SLFixPoint::addition(const SLFixPoint &rhs) const
{
    size_t resultWordLength = std::max(this->m_wl, rhs.m_wl);
    ssize_t resultIntLength = resultWordLength - std::min(this->m_fl, rhs.m_fl);
    //Adding one integer bit to prevent overflow
    resultIntLength++;
    resultWordLength++;
    SLFixPoint tempRHS(resultWordLength, resultIntLength, rhs.m_quantMode, rhs.m_overflowMode);
    SLFixPoint tempLHS(resultWordLength, resultIntLength, this->m_quantMode, this->m_overflowMode);
    tempRHS = rhs;
    tempLHS = *this;

    assert(tempLHS.m_fl == tempRHS.m_fl);
    SLFixPoint result(resultWordLength, resultIntLength, this->m_quantMode, this->m_overflowMode);
    result.m_value = tempLHS.m_value + tempRHS.m_value;

    bool sameSign = (tempLHS.m_value >= 0 && tempRHS.m_value >= 0) || (tempLHS.m_value < 0 && tempRHS.m_value < 0);
    bool wasPositive = (tempLHS.m_value >= 0);
    result.maskAndSignExtend();
    bool isNegative = (result.m_value < 0);

    if (sameSign) {
        //a sign change resulting after same sign operands indicates overflow
        bool didOverflow = (wasPositive && isNegative) || (!wasPositive && !isNegative);
        if (didOverflow) {
            result.handleOverflow();
            switch(m_overflowMode) {
                case OVERFLOW_SATURATE:
                {
                    result.m_value = getSaturatedValue(!wasPositive);
                    break;
                }
                case OVERFLOW_WRAP_AROUND:
                default:
                    break;
            }
        }
    }
    return result;
}

SLFixPoint SLFixPoint::operator+(const SLFixPoint &rhs) const
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

SLFixPoint SLFixPoint::operator-(const SLFixPoint &rhs) const
{
    SLFixPoint temp = rhs;
    temp.m_value = -temp.m_value;
    return addition(temp);
}

SLFixPoint SLFixPoint::operator*(const SLFixPoint &rhs) const
{
    assert(this->m_wl + rhs.m_wl <= sizeof(this->m_value)*8);
    ssize_t intWidth = (this->m_wl - this->m_fl) + (rhs.m_wl - rhs.m_fl);
    SLFixPoint result(this->m_wl + rhs.m_wl, intWidth, this->m_quantMode, this->m_overflowMode);
    result.m_value = (this->m_value * rhs.m_value);
    result.maskAndSignExtend();
    return result;
}

SLFixPoint SLFixPoint::operator/(const SLFixPoint &rhs) const
{
    //TODO implement
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
        long long value = rhs.m_value;
        for (ssize_t i = 0; i < fracDiff; i++) {

            bool signBitSet = value & (1ull << (m_wl - 1));
            bool mostSigBitSet = value & (1ull << (m_wl - 2));
            //if the next single bit shift is going to cause the sign bit to invert we have an overflow
            if ((signBitSet && !mostSigBitSet) || (!signBitSet && mostSigBitSet)) {
                handleOverflow();
                switch(m_overflowMode) {
                    case OVERFLOW_SATURATE:
                    {
                        value = getSaturatedValue(signBitSet);
                        break;
                    }
                    case OVERFLOW_WRAP_AROUND:
                    default:
                        break;
                }
                break;
            }
            value <<= 1;
        }
        this->m_value = value;
    } else { //here we are losing precision
        precisionLossCount++;
        fracDiff = -fracDiff;
        const long long value = rhs.m_value;
        switch (m_quantMode) {
            case QUANT_RND_HALF_UP:
            {
                //shift one bit short of the final amount so that we can take the last bit for rounding
                this->m_value = (value >> (fracDiff - 1));
                int roundBit = this->m_value & 0x01;
                this->m_value >>= 1; //shift that last bit off
                this->m_value += roundBit;
                if (roundBit == 1) {
                    roundUpCount++;
                } else {
                    roundDownCount++;
                }
                break;
            }
            case QUANT_TRUNCATE:
            default:
                this->m_value = (value >> fracDiff);
                break;
        }
        bool wasPositive = (value >= 0);
        bool isNegative = (this->m_value & (1ull << (this->m_wl - 1)));
        unsigned long long mask = ~(~(0ull) >> (sizeof(m_value) * 8 - m_wl));
        long long tempVal = this->m_value;
        if (tempVal < 0) {
            tempVal = -tempVal;
        }
        bool excessBits = (tempVal & mask);
        //either of these three conditions indicates overflow
        bool tooSmall = (value >= 0 && this->m_value == 0) || (value < 0 && (this->m_value == -1 || this->m_value == 0));
        if (!tooSmall) { //if our value was too small such that all sigbits were shifted away, we don't want to count it as overflow
            if ( (wasPositive && isNegative) || (!wasPositive && !isNegative) || excessBits) {
                handleOverflow();
                switch(m_overflowMode) {
                    case OVERFLOW_SATURATE:
                    {
                        this->m_value = getSaturatedValue(!wasPositive);
                        break;
                    }
                    case OVERFLOW_WRAP_AROUND:
                    default:
                        break;
                }
            }
        }
    }
    maskAndSignExtend();
    return *this;
}

SLFixPoint &SLFixPoint::operator=(double val)
{
    assert(m_formatSet);
    this->m_value = static_cast<long long>(val * (1ull << m_fl));

    bool wasPositive = (val >= 0);
    bool isNegative = (this->m_value & (1ull << (this->m_wl - 1)));
    unsigned long long mask = ~(~(0ull) >> (sizeof(m_value) * 8 - m_wl));
    long long tempVal = this->m_value;
    if (tempVal < 0) {
        tempVal = -tempVal;
    }
    if (tempVal == 0 && val != 0.0) {
        underflowCount++; //value was too small to represent with current scaling format
    }
    bool excessBits = (tempVal & mask);
    //if the floating point value is too small, the scaled integer may be 0, in which case
    //a "sign change" is a red herring and not a real overflow
    if (tempVal != 0) {
        if ( (wasPositive && isNegative) || (!wasPositive && !isNegative) || excessBits) {
            handleOverflow();
            switch(m_overflowMode) {
                case OVERFLOW_SATURATE:
                {
                    this->m_value = getSaturatedValue(!wasPositive);
                    break;
                }
                case OVERFLOW_WRAP_AROUND:
                default:
                    break;
            }
        }
    }
    maskAndSignExtend();
    return *this;
}

SLFixPoint &SLFixPoint::operator=(uint64_t val)
{
    assert(m_formatSet);
    unsigned long long mask = ~(~(0ull) >> (sizeof(m_value) * 8 - m_wl));
    bool excessBits = (val & mask);
    assert(!excessBits);
    this->m_value = val;
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

bool SLFixPoint::isFormatSet() const
{
    return m_formatSet;
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
    unsigned long long mask = ~(0ull) >> (sizeof(m_value) * 8 - m_wl);

    return (this->m_value & mask);
}

long long SLFixPoint::getSaturatedValue(bool negative) const
{
    long long value;
    unsigned long long mask = ~(0ull) >> (sizeof(m_value) * 8 - m_wl);
    value = (mask >> 1);
    if (negative) {
        value = -value;
    }
    return value;
}

uint64_t SLFixPoint::slice(size_t end, size_t start) const
{
    assert(start < end);
    assert(end < m_wl);
    long long mask = 0;
    for (size_t i = 0; i <= (end - start); i++) {
        mask |= (1ull << i);
    }
    mask <<= start;
    return (m_value & mask) >> start;
}

void SLFixPoint::maskAndSignExtend()
{
    this->m_value = getMaskedValue();
    extendSign();
}

void SLFixPoint::handleOverflow()
{
    overflowCount++;
    if (m_overflowMode == OVERFLOW_SATURATE) {
        assert(!throwOnOverflow);
    }
    if (throwOnOverflow) {
        //TODO
    }
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
    setFormat(other.m_wl, other.m_wl - other.m_fl, other.m_quantMode, other.m_overflowMode);
}

std::ostream& operator<<(std::ostream& os, const SLFixPoint& obj)
{
    os << obj.to_double();
    return os;
}
