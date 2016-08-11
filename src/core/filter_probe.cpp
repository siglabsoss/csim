
#include <core/filter_probe.hpp>

FilterProbe::FilterProbe(size_t numElements) :
    FilterChainElement("Probe"),
    m_validInput(false),
    m_history(numElements)
{

}

FilterProbe::~FilterProbe()
{

}

bool FilterProbe::input(const filter_io_t &data)
{
    m_validInput = true;
    m_history.push_front(data);
    return true;
}

bool FilterProbe::output(filter_io_t &data)
{
    if (m_validInput) {
        data = m_history[0];
        return true;
    }

    return false;
}

void FilterProbe::tick(void)
{

}

void FilterProbe::clear()
{
    m_history.clear();
}

void FilterProbe::nplot(const std::string &title)
{
    m_p.nplot(m_history, title);
}
void FilterProbe::nplotfft(const std::string &title)
{
    m_p.nplotfft(m_history, title);
}
void FilterProbe::nplotqam(const std::string &title)
{
    m_p.nplotqam(m_history, title);
}
