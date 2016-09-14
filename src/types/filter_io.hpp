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

typedef std::complex<int32_t> ComplexInt;

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
    //filter_io_t & operator=(const ComplexInt &rhs); XXX compiler considers FixedComplex assignment ambiguous with this

    size_t serialize(uint8_t *data) const;
    void deserialize(const uint8_t *data, filter_io_t &output) const;

    ComplexDouble toComplexDouble() const;
};
