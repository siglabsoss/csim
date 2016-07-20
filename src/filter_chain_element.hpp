#pragma once

#include "abstract_siso.hpp"
#include <cstdint>
#include <complex>

enum io_type_t : uint8_t {
    IO_TYPE_NULL = 0,
    IO_TYPE_COMPLEX_DOUBLE,
    IO_TYPE_BYTE
};
struct block_io_t
{
    //Data members
    io_type_t type;
    union {
        std::complex<double> rf;
        uint8_t byte;
    };


    block_io_t() :
        type(IO_TYPE_NULL)
    {}
    ~block_io_t() {}

    block_io_t & operator=(const block_io_t &rhs)
    {
        if (this != &rhs) {
            this->type = rhs.type;
            switch (rhs.type) {
                case IO_TYPE_COMPLEX_DOUBLE:
                    this->rf = rhs.rf;
                    break;
                case IO_TYPE_BYTE:
                    this->byte = rhs.byte;
                    break;
                case IO_TYPE_NULL:
                    break;
            }
        }
        return *this;
    }
};
extern std::ostream& operator<<(std::ostream& os, const block_io_t& obj);

class FilterChainElement : public AbstractSISO< block_io_t, block_io_t >
{
public:
    virtual ~FilterChainElement() {}

    FilterChainElement(FilterChainElement *next) :
        m_next(next)
    {
    }
    FilterChainElement() :
        m_next(nullptr)
    {
    }
    FilterChainElement *m_next;
};
