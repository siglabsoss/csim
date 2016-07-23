#pragma once

#include <core/filter_chain_element.hpp>

class FixedToByte : public FilterChainElement
{
public:
    virtual ~FixedToByte() {}
    FixedToByte() :
        m_counter(0),
        m_outputReady(false)
    {}
    bool input(const block_io_t &data) override
    {
        assert(data.type == IO_TYPE_FIXED_COMPLEX_16);
        m_input = data.fc;
        return true;
    }

    bool output(block_io_t &data) override
    {
        if (m_outputReady) {
            m_outputReady = false;
            data = m_output;
            //std::cout << data << std::endl;
            return true;
        }
        return false;
    }

    void tick(void) override
    {
        m_output.type = IO_TYPE_BYTE;
        m_output.byte = 0xAA;
        m_counter++;
        if (m_counter > 10) {
            m_outputReady = true;
        }
    }
private:
    block_io_t m_output;
    FixedComplex<16> m_input;
    unsigned int m_counter;
    bool m_outputReady;
};
