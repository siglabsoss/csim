#pragma once
#include <iostream>
#include <cassert>

#include <core/filter_chain_element.hpp>

class DummyComplex : public FilterChainElement
{
public:
    virtual ~DummyComplex() {}
    DummyComplex()
    {}
    bool input(const filter_io_t &data) override
    {
        assert(data.type == IO_TYPE_COMPLEX_DOUBLE);
        m_output = data;
        return true;
    }

    bool output(filter_io_t &data) override
    {
        data = m_output;
        std::cout << data << std::endl;
        return true;
    }

    void tick(void) override
    {
        m_output.rf *= 2;
    }
private:
    filter_io_t m_output;
};

class DummyByte : public FilterChainElement
{
public:
    virtual ~DummyByte() {}
    DummyByte()
    {}
    bool input(const filter_io_t &data) override
    {
        assert(data.type == IO_TYPE_BYTE);
        m_output = data;
        return true;
    }

    bool output(filter_io_t &data) override
    {
        data = m_output;
        std::cout << data << std::endl;
        return true;
    }

    void tick(void) override
    {
        m_output.byte++;
    }
private:
    filter_io_t m_output;
};

class DummyComplexToByte : public FilterChainElement
{
public:
    virtual ~DummyComplexToByte() {}
    DummyComplexToByte()
    {}
    bool input(const filter_io_t &data) override
    {
        assert(data.type == IO_TYPE_COMPLEX_DOUBLE);
        m_output = data;
        return true;
    }

    bool output(filter_io_t &data) override
    {
        data = m_output;
        std::cout << data << std::endl;
        return true;
    }

    void tick(void) override
    {
        m_output.type = IO_TYPE_BYTE;
        m_output.byte = static_cast<uint8_t>(m_output.rf.real() + m_output.rf.imag());
    }
private:
    filter_io_t m_output;
};

class DummyByteToComplex : public FilterChainElement
{
public:
    virtual ~DummyByteToComplex() {}
    DummyByteToComplex()
    {}
    bool input(const filter_io_t &data) override
    {
        assert(data.type == IO_TYPE_BYTE);
        m_output = data;
        return true;
    }

    bool output(filter_io_t &data) override
    {
        data = m_output;
        std::cout << data << std::endl;
        return true;
    }

    void tick(void) override
    {
        m_output.type = IO_TYPE_COMPLEX_DOUBLE;
        m_output.rf = m_output.byte;
    }
private:
    filter_io_t m_output;
};

