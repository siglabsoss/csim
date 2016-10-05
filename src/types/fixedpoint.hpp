#pragma once

#define SC_INCLUDE_FX
#include <systemc.h>

template <size_t N, size_t M>
using FixedPoint = sc_dt::sc_fixed <N, M, SC_RND, SC_SAT>;

typedef FixedPoint<32, 4> FixedPoint32;
typedef sc_dt::sc_fix   FixPoint; //run-time configurable version

class SLFixPoint
{
public:
    SLFixPoint();
    SLFixPoint(size_t wordLength, size_t intLength);
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

    void       setFormat(size_t wordLength, size_t intLength);

    uint64_t to_uint64() const;
    int64_t  to_int64()  const;
    double   to_double() const;

protected:
    void                maskValue();
    static long long    signExtendedValue(const SLFixPoint &fp);

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
        this->m_value = static_cast<long long>(val * (1 << m_fl));
        maskValue();
    }

    using SLFixPoint::operator+;
    using SLFixPoint::operator-;
    using SLFixPoint::operator*;
    using SLFixPoint::operator/;

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
