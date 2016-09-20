#pragma once

#include <types/fixedcomplex.hpp>
#include <types/complexdouble.hpp>

enum type_t {
    IO_TYPE_NULL = 0,
    IO_TYPE_COMPLEX_DOUBLE,
    IO_TYPE_FIXED_COMPLEX,
    IO_TYPE_INT32_COMPLEX,
    IO_TYPE_BYTE
};

/**
 * Datatype used to pass integer representation of complex fixed point values between filter.
 *
 * This datatype was created because using a generic SystemC datatype to pass the fixed point data between
 * filters became somewhat cumbersome as the scaling of the type can only be defined during construction
 * of the object. It became more difficult than necessary to reuse the same object to carry fixed point
 * values of different scaling factors.
 */
struct ComplexInt
{
    static constexpr uint32_t SCALE_FACTOR = (1 << 31);
    std::complex<int32_t> c;
    ssize_t               exp;

    ComplexDouble toComplexDouble() const
    {
        double real = static_cast<double>(this->c.real()) / (1ul << (-this->exp + 31));
        double imag = static_cast<double>(this->c.imag()) / (1ul << (-this->exp + 31));
        return ComplexDouble(real, imag);
    }

    double normalizedReal() const
    {
        return static_cast<double>(c.real()) / SCALE_FACTOR;
    }

    double normalizedImag() const
    {
        return static_cast<double>(c.imag()) / SCALE_FACTOR;
    }

    ComplexInt & operator=(const FixedComplex &rhs);
    void assignFixedComplexWithExp(const FixedComplex &val, ssize_t exp);
};

struct filter_io_t
{
    //Data members
    type_t type;
    FixedComplex fc;

    union {
        ComplexDouble rf;
        ComplexInt intc;
        uint8_t byte;
    };

    ~filter_io_t();
    filter_io_t();
    filter_io_t(const filter_io_t &other);

    filter_io_t & operator=(const filter_io_t &rhs);
    filter_io_t & operator=(const ComplexDouble &rhs);
    filter_io_t & operator=(const FixedComplex &rhs);
    filter_io_t & operator=(const uint8_t &rhs);
    filter_io_t & operator=(const ComplexInt &rhs);

    size_t serialize(uint8_t *data) const;
    void deserialize(const uint8_t *data, filter_io_t &output) const;

    ComplexDouble toComplexDouble() const;
};
