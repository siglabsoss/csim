#pragma once

#include <cstdlib>
#include <cstdint>
#include <iostream>

#define DEFAULT_QUANT_MODE SLFixPoint::QUANT_TRUNCATE
#define DEFAULT_OVERFLOW_MODE SLFixPoint::OVERFLOW_WRAP_AROUND

class SLFixPoint
{
public:
    enum quant_mode_t {
        QUANT_TRUNCATE = 0,
        QUANT_RND_HALF_UP
    };
    enum overflow_mode_t {
        OVERFLOW_WRAP_AROUND = 0,
        OVERFLOW_SATURATE
    };
    SLFixPoint();
    SLFixPoint(size_t wordLength, ssize_t intLength);
    SLFixPoint(size_t wordLength, ssize_t intLength, quant_mode_t quantMode, overflow_mode_t overflowMode);
    SLFixPoint(const SLFixPoint &other);
    virtual ~SLFixPoint();

    //Const operators, which return new objects
    SLFixPoint operator+(const SLFixPoint &rhs) const;
    SLFixPoint operator-(const SLFixPoint &rhs) const;
    SLFixPoint operator*(const SLFixPoint &rhs) const;
    SLFixPoint operator/(const SLFixPoint &rhs) const;

    //Self-mutating operators
    SLFixPoint &operator+=(const SLFixPoint &rhs);
    SLFixPoint &operator-=(const SLFixPoint &rhs);
    SLFixPoint &operator<<(size_t shift);
    SLFixPoint &operator>>(size_t shift);

    //Assignment operators
    virtual SLFixPoint &operator=(const SLFixPoint &rhs);
    virtual SLFixPoint &operator=(double val);
    virtual SLFixPoint &operator=(uint64_t val);

    void set(double val);


    void     shiftRadixRight(size_t shiftAmount);
    void     shiftRadixLeft(size_t shiftAmount);

    void     setFormat(size_t wordLength, ssize_t intLength);
    void     setFormat(size_t wordLength, ssize_t intLength, quant_mode_t quantMode, overflow_mode_t overflowMode);
    void     setFormat(const SLFixPoint &other);

    uint64_t to_uint64() const;
    int64_t  to_int64()  const;
    double   to_double() const;
    size_t   wl()  const; //word length
    ssize_t  iwl() const; //integer word length
    bool     isFormatSet() const;
    uint64_t slice(size_t end, size_t start) const;

protected:
    long long           getMaskedValue() const;
    long long           getSaturatedValue(bool negative) const;
    bool                hasExcessBits(long long value) const;
    void                extendSign();
    void                maskAndSignExtend();
    void                handleOverflow();
    SLFixPoint          addition(const SLFixPoint &rhs) const;

public:
    long long       m_value;
    size_t          m_wl;
    size_t          m_fl;
    bool            m_formatSet;
    quant_mode_t    m_quantMode;
    overflow_mode_t m_overflowMode;

    //Global options
    static bool     throwOnOverflow;
    static size_t   overflowCount;
    static size_t   underflowCount;
    static size_t   roundUpCount;
    static size_t   roundDownCount;
    static size_t   precisionLossCount;
};

template <size_t N, size_t M, SLFixPoint::quant_mode_t Q = DEFAULT_QUANT_MODE, SLFixPoint::overflow_mode_t O = DEFAULT_OVERFLOW_MODE>
class SLFixedPoint : public SLFixPoint
{
public:
    SLFixedPoint() :
        SLFixPoint(N, M, Q, O)
    {}

    SLFixedPoint(const SLFixedPoint &other) :
        SLFixPoint(other)
    {
    }

    SLFixedPoint(const SLFixPoint &other) :
        SLFixPoint(other)
    {
    }

    SLFixedPoint(double val) :
        SLFixPoint(N, M, Q, O)
    {
        this->m_value = static_cast<long long>(val * (1ull << m_fl));
    }

    using SLFixPoint::operator+;
    using SLFixPoint::operator-;
    using SLFixPoint::operator*;
    using SLFixPoint::operator/;

    using SLFixPoint::operator<<;
    using SLFixPoint::operator>>;

    SLFixPoint &operator=(double val) override
    {
        return SLFixPoint::operator=(val);
    }

    SLFixPoint &operator=(const SLFixPoint &rhs) override
    {
        return SLFixPoint::operator=(rhs);
    }
    SLFixPoint &operator=(uint64_t val) override
    {
        return SLFixPoint::operator=(val);
    }
};

std::ostream& operator<<(std::ostream& os, const SLFixPoint& obj);
