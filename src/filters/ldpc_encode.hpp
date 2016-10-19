#pragma once


#include <iostream>
#include <cassert>
#include <queue>

#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>

class LDPCEncode : public FilterChainElement
{
public:
    virtual ~LDPCEncode();
    LDPCEncode(const std::vector<std::vector<bool> > &G);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;

    std::vector<bool> encode(const std::vector<bool> &u) const;
private:
    std::vector<std::vector<bool> >    m_G;
    std::queue<bool>                   m_inputBuffer;
};


