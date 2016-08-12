
#include <core/filter_probe.hpp>

FilterProbe::FilterProbe(const std::string &name, size_t numElements, plot_type_t plot) :
    FilterChainElement(name),
    m_validInput(false),
    m_history(numElements),
    m_p(plotter::get()),
    m_didTrigger(false),
    m_plotType(plot),
    m_samplesSinceTrigger(0)
{
}

FilterProbe::FilterProbe(const std::string &name, size_t numElements) :
    FilterChainElement(name),
    m_validInput(false),
    m_history(numElements),
    m_p(plotter::get()),
    m_didTrigger(false),
    m_plotType(PLOT_TYPE_NPLOT),
    m_samplesSinceTrigger(0)
{
}

FilterProbe::~FilterProbe()
{

}

bool FilterProbe::input(const filter_io_t &data)
{
    m_validInput = true;
    m_history.push_front(data);
    if (m_didTrigger) {
        m_samplesSinceTrigger++;
        if ( (m_samplesSinceTrigger >= m_history.capacity()/2) && (m_history.size() == m_history.capacity()) ) {
            plot();
            m_didTrigger = false;
            m_samplesSinceTrigger = 0;
        }
    }
    return true;
}

bool FilterProbe::output(filter_io_t &data)
{
    if (m_validInput) {
        data = m_history[0];
        m_validInput = false;
        return true;
    }

    return false;
}

void FilterProbe::plot()
{
    switch (m_plotType) {
        case PLOT_TYPE_NPLOT:
            return nplot();
        case PLOT_TYPE_NPLOTFFT:
            nplotfft();
            break;
        case PLOT_TYPE_NPLOTQAM:
            nplotqam();
            break;
    }
}

const filter_io_t& FilterProbe::getLatest() const
{
    return m_history[0];
}

size_t FilterProbe::getSize() const
{
    return m_history.size();
}

void FilterProbe::trigger(const std::string &title)
{
    m_triggerName = title;
    m_didTrigger = true;
}

void FilterProbe::nplot()
{
    m_p.nplot(m_history, getName() + " - " + m_triggerName);
}
void FilterProbe::nplotfft()
{
    m_p.nplotfft(m_history, getName() + " - " + m_triggerName);
}
void FilterProbe::nplotqam()
{
    m_p.nplotqam(m_history, getName() + " - " + m_triggerName);
}
