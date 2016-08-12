#pragma once

#include <types/circularbuffer.hpp>
#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>

class FilterProbe : public FilterChainElement
{
public:
    virtual ~FilterProbe();
    FilterProbe(size_t numElements);

    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    virtual void tick(void) = 0;

protected:
    void                            clear();
    void                            nplot(const std::string &title);
    void                            nplotfft(const std::string &title);
    void                            nplotqam(const std::string &title);

    const filter_io_t &             getLatest() const;
    size_t                          getSize() const;

    bool                            m_validInput;
    CircularBuffer<filter_io_t>     m_history;
    const plotter &                 m_p;
private:
};
