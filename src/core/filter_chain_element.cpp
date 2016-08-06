#include <core/filter_chain_element.hpp>
#include <core/logger.hpp>
#include <string>

#include <iomanip>
std::ostream& operator<<(std::ostream& os, const filter_io_t& obj)
{
    switch(obj.type) {
        case IO_TYPE_COMPLEX_DOUBLE:
            os << obj.rf;
            break;
        case IO_TYPE_FIXED_COMPLEX_16:
            os << obj.fc;
            break;
        case IO_TYPE_FIXED_COMPLEX_32:
            os << obj.fc32;
            break;
        case IO_TYPE_FIXED_COMPLEX_16_NEW:
            os << obj.fcn;
            break;
        case IO_TYPE_FIXED_COMPLEX_32_NEW:
            os << obj.fcn32;
            break;
        case IO_TYPE_BYTE:
            os << "[0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(obj.byte) << "]";
            break;
        case IO_TYPE_NULL:
            os << "(NULL DATA)";
            break;
    }
    return os;
}

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
