#include <types/fixedpoint.hpp>
#include <cassert>
#include <algorithm>
#include <climits>

bool   SLFixPoint::throwOnOverflow    = false;
size_t SLFixPoint::overflowCount      = 0;
size_t SLFixPoint::underflowCount     = 0;
size_t SLFixPoint::roundUpCount       = 0;
size_t SLFixPoint::roundDownCount     = 0;
size_t SLFixPoint::precisionLossCount = 0;

SLFixPoint::SLFixPoint() :
    m_value(0),
    m_fmt()
{}

SLFixPoint::SLFixPoint(size_t  wordLength,
                       ssize_t intLength) :
    m_value(0),
    m_fmt()
{
    setFormat(wordLength, intLength, DEFAULT_QUANT_MODE, DEFAULT_OVERFLOW_MODE);
}

SLFixPoint::SLFixPoint(size_t          wordLength,
                       ssize_t         intLength,
                       quant_mode_t    quantMode,
                       overflow_mode_t overflowMode) :
    m_value(0),
    m_fmt()
{
    setFormat(wordLength, intLength, quantMode, overflowMode);
}

SLFixPoint::SLFixPoint(const format_t& fmt) :
    m_value(0),
    m_fmt(fmt)
{
    setFormat(fmt);
}

SLFixPoint::SLFixPoint(const SLFixPoint& other) :
    m_value(other.m_value),
    m_fmt(other.m_fmt)
{}

SLFixPoint::~SLFixPoint()
{}

SLFixPoint SLFixPoint::addition(const SLFixPoint& rhs) const
{
    size_t  resultWordLength = std::max(this->m_fmt.wl, rhs.m_fmt.wl);
    ssize_t resultIntLength  = resultWordLength - std::min(this->m_fmt.fl,
                                                           rhs.m_fmt.fl);

    // Adding one integer bit to prevent overflow
    resultIntLength++;
    resultWordLength++;
    SLFixPoint tempRHS(resultWordLength,
                       resultIntLength,
                       rhs.m_fmt.qm,
                       rhs.m_fmt.om);
    SLFixPoint tempLHS(resultWordLength,
                       resultIntLength,
                       this->m_fmt.qm,
                       this->m_fmt.om);
    tempRHS = rhs;
    tempLHS = *this;

    assert(tempLHS.m_fmt.fl == tempRHS.m_fmt.fl);
    SLFixPoint result(resultWordLength,
                      resultIntLength,
                      this->m_fmt.qm,
                      this->m_fmt.om);
    result.m_value = tempLHS.m_value + tempRHS.m_value;

    bool sameSign =
        (tempLHS.m_value >= 0 &&
         tempRHS.m_value >= 0) || (tempLHS.m_value < 0 && tempRHS.m_value < 0);
    bool wasPositive = (tempLHS.m_value >= 0);
    result.maskAndSignExtend();
    bool isNegative = (result.m_value < 0);

    if (sameSign) {
        // a sign change resulting after same sign operands indicates overflow
        bool didOverflow =
            (wasPositive && isNegative) || (!wasPositive && !isNegative);

        if (didOverflow) {
            result.handleOverflow();

            switch (m_fmt.om) {
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

SLFixPoint SLFixPoint::operator+(const SLFixPoint& rhs) const
{
    return addition(rhs);
}

SLFixPoint& SLFixPoint::operator+=(const SLFixPoint& rhs)
{
    *this = addition(rhs);
    return *this;
}

SLFixPoint& SLFixPoint::operator-=(const SLFixPoint& rhs)
{
    SLFixPoint temp = rhs;

    temp.m_value = -temp.m_value;
    *this        = addition(temp);
    return *this;
}

SLFixPoint SLFixPoint::operator-(const SLFixPoint& rhs) const
{
    SLFixPoint temp = rhs;

    temp.m_value = -temp.m_value;
    return addition(temp);
}

SLFixPoint SLFixPoint::operator*(const SLFixPoint& rhs) const
{
    assert(this->m_fmt.wl + rhs.m_fmt.wl <= sizeof(this->m_value) * 8);
    ssize_t intWidth =
        (this->m_fmt.wl - this->m_fmt.fl) + (rhs.m_fmt.wl - rhs.m_fmt.fl);
    SLFixPoint result(this->m_fmt.wl + rhs.m_fmt.wl,
                      intWidth,
                      this->m_fmt.qm,
                      this->m_fmt.om);
    result.m_value = (this->m_value * rhs.m_value);
    result.maskAndSignExtend();
    return result;
}

SLFixPoint SLFixPoint::operator/(const SLFixPoint& rhs) const
{
    // TODO implement
    return *this;
}

SLFixPoint& SLFixPoint::operator=(const SLFixPoint& rhs)
{
    if (!m_fmt.valid) {
        setFormat(rhs.getFormat());
    }

    // Record sign of value to be assigned
    bool isPositive   = (rhs.m_value >= 0);
    bool willOverflow = false;

    if (rhs.m_value != 0) {
        // Check if we have enough integer bits available
        ssize_t intBitsRequired  = rhs.minNumIntBits();
        ssize_t intBitsAvailable = m_fmt.wl - m_fmt.fl;
        willOverflow = intBitsRequired > intBitsAvailable;
    }

    ssize_t fracDiff      = this->m_fmt.fl - rhs.m_fmt.fl;
    const long long value = rhs.m_value;

    if (fracDiff >= 0) {
        this->m_value = (value << fracDiff);
    } else {
        precisionLossCount++;
        fracDiff = -fracDiff;

        if (m_fmt.qm == QUANT_RND_HALF_UP) {
            // shift one bit short of the final amount so that we can take the
            // last bit for rounding
            this->m_value = (value >> (fracDiff - 1));
            int roundBit = this->m_value & 0x01;
            this->m_value >>= 1; // shift that last bit off
            this->m_value  += roundBit;

            if (roundBit == 1) {
                roundUpCount++;
            } else {
                roundDownCount++;
            }
        } else { // QUANT_TRUNCATE
            this->m_value = (value >> fracDiff);
        }
    }

    if (willOverflow) {
        if (m_fmt.om == OVERFLOW_SATURATE) {
            this->m_value = getSaturatedValue(!isPositive);
        }
        handleOverflow();
    }
    maskAndSignExtend();
    return *this;
}

SLFixPoint& SLFixPoint::operator=(double val)
{
    assert(m_fmt.valid);
    this->m_value = static_cast<long long>(val * (1ull << m_fmt.fl));

    bool wasPositive = (val >= 0);
    bool isNegative  = (this->m_value & (1ull << (this->m_fmt.wl - 1)));

    if ((this->m_value == 0) && (val != 0.0)) {
        underflowCount++; // value was too small to represent with current
                          // scaling format
    }
    bool excessBits = hasExcessBits(m_value);

    // if the floating point value is too small, the scaled integer may be 0, in
    // which case
    // a "sign change" is a red herring and not a real overflow
    if (this->m_value != 0) {
        if ((wasPositive &&
             isNegative) || (!wasPositive && !isNegative) || excessBits) {
            handleOverflow();

            switch (m_fmt.om) {
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

SLFixPoint& SLFixPoint::operator=(uint64_t val)
{
    assert(m_fmt.valid);
    unsigned long long mask = ~(~(0ull) >> (sizeof(m_value) * CHAR_BIT - m_fmt.wl));
    bool excessBits         = (val & mask);
    assert(!excessBits);
    this->m_value = val;
    return *this;
}

SLFixPoint& SLFixPoint::operator<<(size_t shift)
{
    this->m_value <<= shift;
    this->m_fmt.fl += shift;
    return *this;
}

SLFixPoint& SLFixPoint::operator>>(size_t shift)
{
    this->m_value >>= shift;
    this->m_fmt.fl -= shift;
    return *this;
}

bool SLFixPoint::operator==(const SLFixPoint& rhs) const
{
    if (!m_fmt.valid || !rhs.m_fmt.valid) {
        return false;
    }
    return (m_value == rhs.m_value) &&
           (m_fmt.wl == rhs.m_fmt.wl) &&
           (m_fmt.fl == rhs.m_fmt.fl) &&
           (m_fmt.qm == rhs.m_fmt.qm) &&
           (m_fmt.om == rhs.m_fmt.om);
}

void SLFixPoint::shiftRadixRight(size_t shiftAmount)
{
    this->m_fmt.fl += shiftAmount;
}

void SLFixPoint::shiftRadixLeft(size_t shiftAmount)
{
    this->m_fmt.fl -= shiftAmount;
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
    double scale = 1.0 / static_cast<double>(1ull << this->m_fmt.fl);

    return static_cast<double>(this->m_value) * scale;
}

size_t SLFixPoint::wl() const
{
    return m_fmt.wl;
}

ssize_t SLFixPoint::iwl() const
{
    return static_cast<ssize_t>(m_fmt.wl - m_fmt.fl);
}

bool SLFixPoint::isFormatSet() const
{
    return m_fmt.valid;
}

void SLFixPoint::extendSign()
{
    size_t spareBits = sizeof(m_value) * 8 - m_fmt.wl;
    long long val    = m_value;

    val     = (val << spareBits) >> spareBits;
    m_value = val;
}

long long SLFixPoint::getMaskedValue() const
{
    unsigned long long mask = ~(0ull) >> (sizeof(m_value) * 8 - m_fmt.wl);

    return this->m_value & mask;
}

long long SLFixPoint::getSaturatedValue(bool negative) const
{
    long long value;
    unsigned long long mask = ~(0ull) >> (sizeof(m_value) * 8 - m_fmt.wl);

    value = (mask >> 1);

    if (negative) {
        value = ~value; // one's complement because max negative value is one
                        // less than max positive (~value == -value - 1)
    }
    return value;
}

bool SLFixPoint::hasExcessBits(long long value) const
{
    unsigned long long mask = ~(~(0ull) >> (sizeof(value) * 8 - m_fmt.wl));
    long long tempVal       = value;

    if (tempVal < 0) {
        tempVal = -tempVal;
    }
    return static_cast<bool>(tempVal & mask);
}

ssize_t SLFixPoint::minNumIntBits() const
{
    long long value = m_value;

    if (value < 0) {
        value = -value;
    }

    ssize_t numBits = m_fmt.wl - m_fmt.fl;

    unsigned long long mask = (~(0ull) >> (sizeof(value) * 8 - m_fmt.wl));
    value = value & mask;

    while (value != 0 && (value & (1ull << (m_fmt.wl - 1))) == 0) {
        value <<= 1;
        numBits--;
    }
    return numBits + 1; // add one for sign bit
}

uint64_t SLFixPoint::slice(size_t end, size_t start) const
{
    assert(start < end);
    assert(end < m_fmt.wl);
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

    if (m_fmt.om == OVERFLOW_SATURATE) {
        assert(!throwOnOverflow);
    }

    if (throwOnOverflow) {
        // TODO
    }
}

void SLFixPoint::setFormat(size_t          wordLength,
                           ssize_t         intLength,
                           quant_mode_t    quantMode,
                           overflow_mode_t overflowMode)
{
        assert(static_cast<ssize_t>(wordLength) >= intLength);
        assert(wordLength <= sizeof(m_value) * 8);

    m_fmt.qm = quantMode;
    m_fmt.om = overflowMode;

    // intLength can be negative to represent small values with
    // higher precision. thus m_fmt.fl can be greater than m_fmt.wl
    m_fmt.wl    = wordLength;
    m_fmt.fl    = wordLength - intLength;
    m_fmt.valid = true;
}

void SLFixPoint::setFormat(const SLFixPoint::format_t& fmt)
{
    m_fmt = fmt;
}

SLFixPoint::format_t SLFixPoint::getFormat() const
{
    return m_fmt;
}

std::ostream& operator<<(std::ostream& os, const SLFixPoint& obj)
{
    os << obj.to_double();
    return os;
}
