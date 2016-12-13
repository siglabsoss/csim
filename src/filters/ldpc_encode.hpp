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
    bool output(filter_io_t &data) override;
    void tick(void) override;

    size_t getMsgLen() const;
    size_t getCwLen() const;

private:
    std::vector<bool> encode(const std::vector<bool> &u) const;

    std::vector<std::vector<bool> >    m_G;
    std::queue<bool>                   m_outputBuffer;
};


