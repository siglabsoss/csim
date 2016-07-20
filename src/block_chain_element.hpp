#pragma once

#include "abstract_siso.hpp"
#include <vector>


enum io_type_t : uint8_t {
    IO_TYPE_NULL = 0,
    IO_TYPE_COMPLEX_DOUBLE,
    IO_TYPE_BYTE
};
struct block_io_t
{
    block_io_t() :
        type(IO_TYPE_NULL)
    {}
    ~block_io_t() {}
    io_type_t type;
    union {
        std::complex<double> rf;
        uint8_t byte;
    };
    block_io_t & operator=(const block_io_t &rhs)
    {
        if (this != &rhs) {
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

class BlockChainElement : public AbstractSISO< block_io_t, block_io_t >
{
public:
    virtual ~BlockChainElement() {}

    BlockChainElement(BlockChainElement *next) :
        m_next(next)
    {
    }
    BlockChainElement() :
        m_next(nullptr)
    {
    }
    BlockChainElement *m_next;
};
