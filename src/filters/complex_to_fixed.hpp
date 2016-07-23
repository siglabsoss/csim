#pragma once

#include <core/filter_chain_element.hpp>

class ComplexToFixed : public FilterChainElement
{
public:
    virtual ~ComplexToFixed() {}
    ComplexToFixed()
    {}
    bool input(const block_io_t &data) override
    {
        assert(data.type == IO_TYPE_COMPLEX_DOUBLE);
        m_input = data.rf;
        return true;
    }

    bool output(block_io_t &data) override
    {
        data = m_output;
        //std::cout << data << std::endl;
        return true;
    }

    void tick(void) override
    {
        m_output.type = IO_TYPE_FIXED_COMPLEX_16;
        m_output.fc = FixedComplex<16>(sc_int<16>(static_cast<int>(m_input.real() * 32768)), sc_int<16>(static_cast<int>(m_input.imag() * 32768)));
    }
private:
    block_io_t m_output;
    std::complex<double> m_input;
};
