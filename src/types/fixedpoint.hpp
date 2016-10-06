#include <cstdlib>
#include <cstdint>
#include <iostream>

#pragma once

class SLFixPoint
{
public:
    SLFixPoint();
    SLFixPoint(size_t wordLength, ssize_t intLength);
    SLFixPoint(const SLFixPoint &other);
    virtual ~SLFixPoint();

    SLFixPoint operator+(const SLFixPoint &rhs);
    SLFixPoint operator-(const SLFixPoint &rhs);
    SLFixPoint operator*(const SLFixPoint &rhs);
    SLFixPoint operator/(const SLFixPoint &rhs);

    virtual SLFixPoint &operator=(const SLFixPoint &rhs);
    virtual SLFixPoint &operator=(double val);
    SLFixPoint &operator<<(size_t shift);
    SLFixPoint &operator>>(size_t shift);

    void     shiftRadixRight(size_t shiftAmount);
    void     shiftRadixLeft(size_t shiftAmount);

    void     setFormat(size_t wordLength, ssize_t intLength);

    uint64_t to_uint64() const;
    int64_t  to_int64()  const;
    double   to_double() const;
    size_t   wl() const; //word length
    ssize_t  iwl() const; //integer word length

protected:
    long long           getMaskedValue() const;
    void                extendSign();
    void                maskAndSignExtend();
    SLFixPoint          addition(const SLFixPoint &rhs);

public:
    long long   m_value;
    size_t      m_wl;
    size_t      m_fl;
};

template <size_t N, size_t M>
class SLFixedPoint : public SLFixPoint
{
public:
    SLFixedPoint() :
        SLFixPoint(N, M)
    {}

    SLFixedPoint(const SLFixedPoint &other) :
        SLFixPoint(other)
    {

    }

    SLFixedPoint(double val) :
        SLFixPoint(N, M)
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
};

std::ostream& operator<<(std::ostream& os, const SLFixPoint& obj);
