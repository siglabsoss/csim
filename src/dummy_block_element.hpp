#pragma once

#include "block_chain_element.hpp"

class DummyBlockElement : public BlockChainElement
{
public:
    virtual ~DummyBlockElement() {}
    DummyBlockElement(BlockChainElement *next) :
        BlockChainElement(next)
    {}
    bool input(const block_io_t &data)
    {
        return true;
    }

    bool output(block_io_t &data)
    {
        return true;
    }
};
