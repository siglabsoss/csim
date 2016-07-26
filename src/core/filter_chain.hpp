#pragma once

#include <core/filter_chain_element.hpp>
#include <core/publisher.hpp>


class FilterChain : public AbstractSISO < filter_io_t, filter_io_t >
{

public:
    FilterChain();
    FilterChain(const FilterChain &other);
    virtual ~FilterChain() {}

    void init();

    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick() override;

    FilterChain & operator=(const FilterChainElement &rhs);

private: //methods
    void publish(FilterChainElement *current);

private:
    FilterChainElement *    m_head;
    filter_io_t             m_output;
    bool                    m_outputReady;
};
