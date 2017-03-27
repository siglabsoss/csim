#include <core/filter_chain_element.hpp>
#include <core/logger.hpp>
#include <string>

const std::string& FilterChainElement::getName() const
{
    return m_name;
}

bool FilterChainElement::hasInputFIFO() const
{
    return m_fifo.capacity() > 0;
}

double FilterChainElement::inputFIFOUtilization() const
{
    if (m_fifo.capacity() == 0) {
        return 1.0;
    }
    return static_cast<double>(m_fifo.size()) /
           static_cast<double>(m_fifo.capacity());
}

bool FilterChainElement::input(const filter_io_t& data)
{
    if (m_fifo.full()) {
        return false;
    }
    m_fifo.push_back(data);
    m_gotInput = true;
    return true;
}

void FilterChainElement::setDebug(bool flag)
{
    m_debug = flag;
}

unsigned int FilterChainElement::instanceCount = 0;

FilterChainElement::FilterChainElement(std::string name,
                                       size_t      inputFIFOSize) :
    m_next(nullptr),
    m_name(name + std::to_string(instanceCount)),
    m_gotInput(false),
    m_debug(false),
    m_fifo(inputFIFOSize)
{
    // log_info("New filter chain element with name = %s", m_name.c_str());
    instanceCount++;
}
