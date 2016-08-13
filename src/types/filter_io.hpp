#pragma once

#include <types/fixedcomplex.hpp>
#include <types/complexdouble.hpp>

enum type_t {
    IO_TYPE_NULL = 0,
    IO_TYPE_COMPLEX_DOUBLE,
    IO_TYPE_FIXED_COMPLEX_16,
    IO_TYPE_FIXED_COMPLEX_32,
    IO_TYPE_FIXED_COMPLEX_16_NEW,
    IO_TYPE_FIXED_COMPLEX_32_NEW,
    IO_TYPE_BYTE
};

struct filter_io_t
{
    //Data members
    type_t type;
    FixedComplex<16> fc;
    FixedComplex<32> fc32;
    FixedComplex16 fcn;
    FixedComplex32 fcn32;
    union {
        ComplexDouble rf;
        uint8_t byte;
    };

    ~filter_io_t();
    filter_io_t();
    filter_io_t(const filter_io_t &other);

    filter_io_t & operator=(const filter_io_t &rhs);
    filter_io_t & operator=(const ComplexDouble &rhs);
    filter_io_t & operator=(const FixedComplex<16> &rhs);
    filter_io_t & operator=(const FixedComplex<32> &rhs);
    filter_io_t & operator=(const FixedComplex16 &rhs);
    filter_io_t & operator=(const FixedComplex32 &rhs);
    filter_io_t & operator=(const uint8_t &rhs);

    size_t serialize(uint8_t *data) const;
    void deserialize(const uint8_t *data, filter_io_t &output) const;

    ComplexDouble toComplexDouble() const;
};