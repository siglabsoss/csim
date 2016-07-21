#pragma once

#include "filter_chain_element.hpp"

class FilterChain : public AbstractSISO < block_io_t, block_io_t >
{

public:
    FilterChain();

    virtual ~FilterChain() {}

    bool input(const block_io_t &data) override;
    bool output(block_io_t &data) override;
    void tick() override;

    FilterChain & operator=(const FilterChainElement &rhs);

private:
    FilterChainElement *    m_head;
    block_io_t              m_output;
    bool                    m_outputReady;
};
