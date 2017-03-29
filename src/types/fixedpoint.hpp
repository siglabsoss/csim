/**
 * @file fixedpoint.hpp Generic fixed-point arithmetic implementation
 */

#pragma once

#include <cstdlib>
#include <cstdint>
#include <iostream>

#define DEFAULT_QUANT_MODE SLFixPoint::QUANT_TRUNCATE
#define DEFAULT_OVERFLOW_MODE SLFixPoint::OVERFLOW_WRAP_AROUND

class SLFixComplex;

/**
 * SLFixPoint - class which encapsulates a single signed fixed-point value.
 * This class supports signed fixed-point values with arbitrary bit widths
 * and fraction lengths with a maximum width equal to the largest word
 * size available for the machine.
 */
class SLFixPoint
{
public:

    /**
     * quant_mode_t options for quantization (rounding) behavior
     */
    enum quant_mode_t {
        QUANT_TRUNCATE = 0,       // / truncate excess bits
        QUANT_RND_HALF_UP         // / round towards positive at half
    };
    enum overflow_mode_t {
        OVERFLOW_WRAP_AROUND = 0, // / wrap around (discarding MSBs)
        OVERFLOW_SATURATE         // / use max positive or max negative value
    };

    /**
     * format_t the set of parameters to define the fixed-point format
     */
    struct format_t
    {
        format_t() :
            wl(0),
            fl(0),
            qm(DEFAULT_QUANT_MODE),
            om(DEFAULT_OVERFLOW_MODE),
            valid(false)
        {}

        size_t          wl;    // / word length (total bit width of value)
        size_t          fl;    // / fraction length (number of fraction bits)
        quant_mode_t    qm;    // / the particular quantization mode
        overflow_mode_t om;    // / the particular overlflow mode
        bool            valid; // / whether the format contained is valid
    };

    /**
     * SLFixPoint default constructor.
     * The object will need a format set before any operations can be performed
     * or any value set. This can be done by assigning the object to another or
     * by setting the format after construction.
     *
     * @see setFormat()
     */
    SLFixPoint();

    /**
     * SLFixPoint basic constructor
     * @param wordLength total bit width of value
     * @param intLength length of integer portion of value (including sign bit)
     */
    SLFixPoint(size_t  wordLength,
               ssize_t intLength);

    /**
     * SLFixPoint complete constructor
     * @param wordLength total bit width of value
     * @param intLength length of integer portion of value (including sign bit)
     * @param quantMode desired quantization behavior
     * @param overflowMode desired overflow behavior
     */
    SLFixPoint(size_t          wordLength,
               ssize_t         intLength,
               quant_mode_t    quantMode,
               overflow_mode_t overflowMode);

    /**
     * SLFixPoint copy constructor
     * @param other the other object to copy
     */
    SLFixPoint(const SLFixPoint& other);

    /**
     * SLFixPoint complete constructor with format struct
     * @param fmt the predefined fixed-point format
     */
    SLFixPoint(const format_t& fmt);
    virtual ~SLFixPoint();

    // Const operators, which return new objects

    /**
     * operator+ fixed-point addition
     * @param rhs fixed-point value of right hand side of addition operator
     * @return newly constructed fixed-point value with result of addition.
     * The word length of the result will equal the maximum of the two
     * operands plus one to eliminate the possibility of overflow. The integer
     * length will be the maximum integer length of the two operands plus one.
     */
    SLFixPoint operator+(const SLFixPoint& rhs) const;

    /**
     * operator- fixed-point subtraction
     * @param rhs fixed-point value of right hand side of subtraction operator
     * @return newly constructed fixed-point value with result of subtraction.
     * The word length of the result will equal the maximum of the two
     * operands plus one to eliminate the possibility of overflow. The integer
     * length will be the maximum integer length of the two operands plus one.
     */
    SLFixPoint operator-(const SLFixPoint& rhs) const;

    /**
     * operator* fixed-point multiplication
     * @param rhs fixed-point value of right hand side of multiplication
     * operator
     * @return newly constructed fixed-point value with result of
     * multiplication. The word length of the result will equal the sum of the
     * two operands. The integer length will be the sum of the integer lengths
     * of the operands.
     */
    SLFixPoint operator*(const SLFixPoint& rhs) const;

    /**
     * operator/ fixed-point division
     * @param rhs fixed-point value of right hand side of division operator.
     * This method is currently not implemented
     */
    SLFixPoint operator/(const SLFixPoint& rhs) const;

    // Self-mutating operators

    /**
     * operator+= fixed-point incremental addition operator
     * @param rhs fixed-point value of right hand side of operator
     * @return reference to current object after result of operation
     */
    SLFixPoint& operator+=(const SLFixPoint& rhs);

    /**
     * operator-= fixed-point incremental subtraction operator
     * @param rhs fixed-point value of right hand side of operator
     * @return reference to current object after result of operation
     */
    SLFixPoint& operator-=(const SLFixPoint& rhs);

    /**
     * operator<< left-shift operator
     * Performs a left shift on the integer. The scaling (fraction length) is
     * increased by the number of shifts.
     * @param shift number of bits to shift left
     * @return a reference to `this` after the operation
     */
    SLFixPoint& operator<<(size_t shift);

    /**
     * operator<< right-shift operator
     * Performs a right shift on the integer. The scaling (fraction length) is
     * decreased by the number of shifts.
     * @param shift number of bits to shift right
     * @return a reference to `this` after the operation
     */
    SLFixPoint& operator>>(size_t shift);

    // Assignment operators

    /**
     * operator= assignment from fixed-point value
     * Copies the value on the right hand side of the operation. If this object
     * does not have a format set, it is inherited from the other object. If
     * this object does have a format set, the other object is cast to this
     * format before assignment.
     * @param rhs fixed-point value of right hand side of operator
     * @return a reference to `this` after the operation.
     */
    virtual SLFixPoint& operator=(const SLFixPoint& rhs);

    /**
     * operator= assignment from complex fixed-point value
     * Copies the real component of the complex value to this object. The
     * same assignment rules as assignment from fixed-point value are followed.
     * @param rhs fixed-point value of right hand side of operator
     * @return a reference to `this` after the operation.
     */
    virtual SLFixPoint& operator=(const SLFixComplex& rhs);

    /**
     * operator= assignment from double-precision floating point value
     * The current object must have a format predefined. The floating point
     * value is then scaled appropriately based on the fraction length. The
     * result is then quantized and checked for overflow.
     * @param val the double-precision floating point value to set
     * @return a reference to `this` after the operation.
     */
    virtual SLFixPoint& operator=(double val);

    /**
     * operator= assignment from integer value
     * The current object must have a format predefined. The integer value
     * is then used as the underlying bit representation of the fixed-point
     * value. The result is checked for overflow.
     * @param val the integer value to set
     * @return a reference to `this` after the operation.
     */
    virtual SLFixPoint& operator=(uint64_t val);

    /**
     * operator== equality check operator
     * Equality is determined by checking the underlying integer values for
     * equality as well as all of the format attributes.
     * @param rhs fixed-point value of right hand side of operator
     * @return true if the right hand side of operator equals this object.
     */
    bool                operator==(const SLFixPoint& rhs) const;

    /**
     * set alternate method for assisgnment from double-precision floating
     * point value. This method exists solely for the purpose of
     * interoperability with Python.
     * @param val double-precision floating point value to set
     */
    void                set(double val);

    /**
     * shiftRadixRight shift the location of the radix point to the right.
     * In other words, shrink the number of fraction bits while keeping the
     * total word length the same.
     * @param shiftAmount an integer representing the number of times to
     * shift right.
     */
    void                shiftRadixRight(size_t shiftAmount);

    /**
     * shiftRadixLeft shift the location of the radix point to the left.
     * In other words, increase the number of fraction bits while keeping the
     * total word length the same.
     * @param shiftAmount an integer representing the number of times to shift
     * left.
     */
    void                shiftRadixLeft(size_t shiftAmount);

    /**
     * setFormat set the format of the fixed-point value.
     * @param wordLength total bit width of value
     * @param intLength length of integer portion of value (including sign bit)
     * @param quantMode desired quantization behavior
     * @param overflowMode desired overflow behavior
     */
    void                setFormat(size_t          wordLength,
                                  ssize_t         intLength,
                                  quant_mode_t    quantMode,
                                  overflow_mode_t overflowMode);

    /**
     * setFormat set the format of the fixed-point value.
     * @param fmt the predefined fixed-point format
     */
    void     setFormat(const format_t& fmt);

    /**
     * getFormat retrieve the format of the fixed-point value
     * @return A format_t structure representing the format.
     */
    format_t getFormat() const;

    /**
     * to_uint64 retrieve the underlying bit value as an unsigned, 64-bit int
     * @return the underyling bit value as an unsigned, 64-bit int
     */
    uint64_t to_uint64() const;

    /**
     * to_int64 retrieve the underlying bit value as a signed, 64-bit int
     * @return the underlying bit value as a signed, 64-bit int
     */
    int64_t  to_int64()  const;

    /**
     * to_double retrieve a double-precision floating point representation
     * of the fixed-point value.
     * @return a double-precision floating point number representing the
     * underlying fixed-point value.
     */
    double   to_double() const;

    /**
     * wl retrieve the word length of the fixed-point value.
     * @return an integer representing the length of the fixed-point value.
     */
    size_t   wl()  const; // word length

    /**
     * iwl retrieve length of integer portion of fixed-point value, which
     * includes the sign bit.
     * @return an integer representing the length of the integer portion
     */
    ssize_t  iwl() const; // integer word length

    /**
     * isFormatSet check if the fixed-point format of this object is set.
     * @return true if the format has been set.
     */
    bool     isFormatSet() const;

    /**
     * slice retrieve a portion of the underlying integer bit value. The least
     * significant bit is indexed at 0
     * @param end the index of the most significant bit
     * @param start the index of the least significant bit
     * @return an unsigned integer representing the subsection of the
     * fixed-point integer value.
     */
    uint64_t slice(size_t end,
                   size_t start) const;

protected:

    long long  getMaskedValue() const;
    long long  getSaturatedValue(bool negative) const;
    bool       hasExcessBits(long long value) const;
    ssize_t    minNumIntBits() const;
    void       extendSign();
    void       maskAndSignExtend();
    void       handleOverflow();
    SLFixPoint addition(const SLFixPoint& rhs) const;

    long long m_value;
    format_t  m_fmt;

public:

    // Global options
    static bool   throwOnOverflow;
    static size_t overflowCount;
    static size_t underflowCount;
    static size_t roundUpCount;
    static size_t roundDownCount;
    static size_t precisionLossCount;
};

template<size_t N, size_t M, SLFixPoint::quant_mode_t Q = DEFAULT_QUANT_MODE,
         SLFixPoint::overflow_mode_t O                  = DEFAULT_OVERFLOW_MODE>
class SLFixedPoint : public SLFixPoint
{
public:

    SLFixedPoint() :
        SLFixPoint(N, M, Q, O)
    {}

    SLFixedPoint(const SLFixedPoint& other) :
        SLFixPoint(other)
    {}

    SLFixedPoint(const SLFixPoint& other) :
        SLFixPoint(other)
    {}

    SLFixedPoint(double val) :
        SLFixPoint(N, M, Q, O)
    {
        this->m_value = static_cast<long long>(val * (1ull << m_fmt.fl));
    }

    using SLFixPoint::operator+;
    using SLFixPoint::operator-;
    using SLFixPoint::operator*;
    using SLFixPoint::operator/;

    using SLFixPoint::operator<<;
    using SLFixPoint::operator>>;

    SLFixPoint& operator=(double val) override
    {
        return SLFixPoint::operator=(val);
    }

    SLFixPoint& operator=(const SLFixPoint& rhs) override
    {
        return SLFixPoint::operator=(rhs);
    }

    SLFixPoint& operator=(uint64_t val) override
    {
        return SLFixPoint::operator=(val);
    }
};

std::ostream& operator<<(std::ostream    & os,
                         const SLFixPoint& obj);
