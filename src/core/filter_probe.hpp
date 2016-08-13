#pragma once

#include <types/circularbuffer.hpp>
#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>

class FilterProbe : public FilterChainElement
{
public:
    enum plot_type_t {
        PLOT_TYPE_NPLOT = 0,
        PLOT_TYPE_NPLOTFFT,
        PLOT_TYPE_NPLOTQAM
    };
    virtual ~FilterProbe();
    FilterProbe(const std::string &name, size_t numElements);
    FilterProbe(const std::string &name, size_t numElements, plot_type_t plot);

    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    virtual void tick(void) = 0;

protected: //methods
    void                            trigger(const std::string &title);

    const filter_io_t &             getLatest() const;
    size_t                          getSize() const;


    bool                            m_validInput;
    CircularBuffer<filter_io_t>     m_history;
    const plotter &                 m_p;

private: //methods
    void                            plot();
    void                            nplot();
    void                            nplotfft();
    void                            nplotqam();

private: //members
    bool                            m_didTrigger;
    plot_type_t                     m_plotType;
    size_t                          m_samplesSinceTrigger;
    std::string                     m_triggerName;
};
