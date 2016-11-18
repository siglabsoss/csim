#pragma once

#include <types/circularbuffer.hpp>
#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>

class FilterProbe : public FilterChainElement
{
public:
    enum data_dump_t {
        NPLOT = 0,
        NPLOTFFT,
        NPLOTQAM,
        CSV
    };
    virtual ~FilterProbe();
    FilterProbe(const std::string &name, size_t numElements);
    FilterProbe(const std::string &name, size_t numElements, data_dump_t outType);

    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    virtual void tick(void) = 0;

protected: //methods
    void                            trigger(const std::string &title);

    const filter_io_t &             getLatest() const;
    size_t                          getSize() const;


    bool                            m_validInput;
    bool                            m_didReceiveNonzero;
    CircularBuffer<filter_io_t>     m_history;
    const plotter &                 m_p;

private: //methods
    void                            dump();
    void                            nplot();
    void                            nplotfft();
    void                            nplotqam();
    void                            csv();

private: //members
    bool                            m_didTrigger;
    data_dump_t                     m_outType;
    size_t                          m_samplesSinceTrigger;
    std::string                     m_triggerName;
};
