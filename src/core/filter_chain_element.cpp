#include <core/filter_chain_element.hpp>
#include <core/logger.hpp>
#include <string>

const std::string &FilterChainElement::getName() const
{
    return m_name;
}

bool FilterChainElement::shouldPublish() const
{
    return m_shouldPublish;
}

void FilterChainElement::shouldPublish(bool desired)
{
    m_shouldPublish = desired;
}

unsigned int FilterChainElement::instanceCount = 0;

FilterChainElement::FilterChainElement(std::string name) :
    m_next(nullptr),
    m_name(name + std::to_string(instanceCount)),
    m_shouldPublish(false)
{
    log_info("New filter chain element with name = %s", m_name.c_str());
    instanceCount++;
}
