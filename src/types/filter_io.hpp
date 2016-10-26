#pragma once

#include <types/fixedcomplex.hpp>
#include <types/complexdouble.hpp>

enum type_t {
    IO_TYPE_NULL = 0,
    IO_TYPE_COMPLEX_DOUBLE,
    IO_TYPE_COMPLEX_FIXPOINT,
    IO_TYPE_FIXPOINT,
    IO_TYPE_BYTE,
    IO_TYPE_BIT
};

struct filter_io_t
{
    //Data members
    type_t type;
    union {
        ComplexDouble   rf;
        SLFixComplex    fc;
        SLFixPoint      fp;
        uint8_t         byte;
        bool            bit;
    };

    ~filter_io_t();
    filter_io_t();
    filter_io_t(const filter_io_t &other);

    filter_io_t & operator=(const filter_io_t &rhs);
    filter_io_t & operator=(const ComplexDouble &rhs);
    filter_io_t & operator=(const SLFixComplex &rhs);
    filter_io_t & operator=(const SLFixPoint &rhs);
    filter_io_t & operator=(const uint8_t &rhs);
    filter_io_t & operator=(const bool &rhs);

    size_t serialize(uint8_t *data) const;
    void deserialize(const uint8_t *data, filter_io_t &output) const;

    ComplexDouble toComplexDouble() const;
};
