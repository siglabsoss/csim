
#include <core/filter_probe.hpp>
#include <fstream>

FilterProbe::FilterProbe(const std::string &name, size_t numElements, data_dump_t outType) :
    FilterChainElement(name),
    m_validInput(false),
    m_didReceiveNonzero(false),
    m_history(numElements),
    m_p(plotter::get()),
    m_didTrigger(false),
    m_outType(outType),
    m_samplesSinceTrigger(0)
{
}

FilterProbe::FilterProbe(const std::string &name, size_t numElements) :
    FilterChainElement(name),
    m_validInput(false),
    m_didReceiveNonzero(false),
    m_history(numElements),
    m_p(plotter::get()),
    m_didTrigger(false),
    m_outType(NPLOT),
    m_samplesSinceTrigger(0)
{
}

FilterProbe::~FilterProbe()
{

}

bool FilterProbe::input(const filter_io_t &data)
{
    if (!m_didReceiveNonzero) {
        switch(data.type) {
            case IO_TYPE_COMPLEX_DOUBLE:
                m_didReceiveNonzero =  (std::abs(data.rf) > 0.0);
                break;
            case IO_TYPE_COMPLEX_FIXPOINT:
                m_didReceiveNonzero = (data.fc.real().to_int64() != 0) || (data.fc.imag().to_int64() != 0);
                break;
            case IO_TYPE_FIXPOINT:
                m_didReceiveNonzero = (data.fp.to_int64() != 0);
                break;
            case IO_TYPE_BYTE:
                m_didReceiveNonzero = (data.byte != 0);
                break;
            case IO_TYPE_BIT:
                m_didReceiveNonzero = (data.bit != 0);
                break;
            case IO_TYPE_NULL:
                break;
        }
    }
    m_validInput = true;
    m_history.push_back(data);
    return true;
}

bool FilterProbe::output(filter_io_t &data)
{
    if (m_didTrigger) {
        dump();
        m_didTrigger = false;
    }
    if (m_validInput) {
        data = getLatest();
        m_validInput = false;
        return true;
    }

    return false;
}

void FilterProbe::dump()
{
    switch (m_outType) {
        case NPLOT:
            return nplot();
        case NPLOTFFT:
            nplotfft();
            break;
        case NPLOTQAM:
            nplotqam();
            break;
        case CSV:
            csv();
            break;
    }
}

const filter_io_t& FilterProbe::getLatest() const
{
    return m_history[m_history.size()-1];
}

size_t FilterProbe::getSize() const
{
    return m_history.size();
}

void FilterProbe::trigger(const std::string &title)
{
    std::stringstream ss;
    ss << m_name << "_" << title;
    m_triggerName = ss.str();
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
void FilterProbe::csv()
{
    std::ofstream ofs;
    std::stringstream filename;
    filename << m_triggerName << ".csv";
    ofs.open(filename.str().c_str(), std::ofstream::out | std::ofstream::trunc);
    for (auto it = m_history.begin(); it != m_history.end(); ++it) {
        switch(it->type) {
            case IO_TYPE_COMPLEX_DOUBLE:
            case IO_TYPE_COMPLEX_FIXPOINT:
            {
                ComplexDouble val = it->toComplexDouble();
                ofs << val.real() << "," << val.imag() << std::endl;
                break;
            }
            case IO_TYPE_NULL:
            case IO_TYPE_FIXPOINT:
            case IO_TYPE_BYTE:
            case IO_TYPE_BIT:
            default:
                ofs << *it;
        }
    }
}
