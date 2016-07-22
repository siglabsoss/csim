#include <core/filter_chain_element.hpp>

#include <iomanip>
std::ostream& operator<<(std::ostream& os, const block_io_t& obj)
{
    switch(obj.type) {
        case IO_TYPE_COMPLEX_DOUBLE:
            os << obj.rf;
            break;
        case IO_TYPE_FIXED_COMPLEX_16:
            os << obj.fc;
            break;
        case IO_TYPE_BYTE:
            os << "[0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(obj.byte) << "]";
            break;
        case IO_TYPE_NULL:
            break;
    }
    return os;
}
