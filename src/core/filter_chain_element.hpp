#pragma once

#include <interfaces/abstract_siso.hpp>
#include <types/fixedcomplex.hpp>
#include <types/complexdouble.hpp>
#include <types/filter_io.hpp>
#include <types/circularbuffer.hpp>
#include <memory>

#include <cstdint>


extern std::ostream& operator<<(std::ostream& os, const filter_io_t& obj);

class FilterChainElement : public AbstractSISO< filter_io_t, filter_io_t >
{
public:
    virtual ~FilterChainElement() {}
    FilterChainElement(std::string name = std::string("NONAME"), size_t inputFIFOSize = 0);

    friend FilterChainElement& operator+(const FilterChainElement &lhs, FilterChainElement &rhs)
    {
        rhs.m_next.reset((FilterChainElement *)&lhs);
        return rhs;
    }

    const std::string   &getName() const;
    bool                shouldPublish() const;
    void                shouldPublish(bool desired);

    virtual bool        input(const filter_io_t &data) override;
    bool                hasInputFIFO() const;
    double              inputFIFOUtilization() const;

    void                setDebug(bool flag);

    std::unique_ptr<FilterChainElement> m_next;
protected:
    std::string         m_name;
    bool                m_gotInput;
    bool                m_debug;
    CircularBuffer<filter_io_t> m_fifo;

    static unsigned int instanceCount;
};
