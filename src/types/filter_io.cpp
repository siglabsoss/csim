#include <types/filter_io.hpp>

#include <iomanip>
#include <cassert>
#include <cstring>

std::ostream& operator<<(std::ostream& os, const filter_io_t& obj)
{
    switch(obj.type) {
        case IO_TYPE_COMPLEX_DOUBLE:
            os << obj.rf;
            break;
        case IO_TYPE_COMPLEX_FIXPOINT:
            os << obj.fc;
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

filter_io_t::filter_io_t() :
    type(IO_TYPE_NULL)
{}

filter_io_t::filter_io_t(const filter_io_t &other) :
        type(other.type)
{
    if (this != &other) {
        switch (other.type) {
            case IO_TYPE_COMPLEX_DOUBLE:
                this->rf = other.rf;
                break;
            case IO_TYPE_COMPLEX_FIXPOINT:
                this->fc.setFormat(other.fc);
                this->fc = other.fc;
                break;
            case IO_TYPE_BYTE:
                this->byte = other.byte;
                break;
            case IO_TYPE_NULL:
                break;
        }
    }
}

filter_io_t & filter_io_t::operator=(const filter_io_t &rhs)
{
    if (this != &rhs) {
        this->type = rhs.type;
        switch (rhs.type) {
            case IO_TYPE_COMPLEX_DOUBLE:
                this->rf = rhs.rf;
                break;
            case IO_TYPE_COMPLEX_FIXPOINT:
                this->fc.setFormat(rhs.fc);
                this->fc = rhs.fc;
                break;
            case IO_TYPE_BYTE:
                this->byte = rhs.byte;
                break;
            case IO_TYPE_NULL:
                break;
        }
    }
    return *this;
}

filter_io_t & filter_io_t::operator=(const ComplexDouble &rhs)
{
    this->type = IO_TYPE_COMPLEX_DOUBLE;
    this->rf = rhs;
    return *this;
}

filter_io_t & filter_io_t::operator=(const SLFixComplex &rhs)
{
    this->type = IO_TYPE_COMPLEX_FIXPOINT;
    this->fc.setFormat(rhs);
    this->fc = rhs;
    return *this;
}

filter_io_t & filter_io_t::operator=(const uint8_t &rhs)
{
    this->type = IO_TYPE_BYTE;
    this->byte = rhs;
    return *this;
}

ComplexDouble filter_io_t::toComplexDouble() const
{
    double real = 0, imag = 0;
    switch(this->type) {
        case IO_TYPE_NULL:
            break;
        case IO_TYPE_COMPLEX_DOUBLE:
            real = this->rf.real();
            imag = this->rf.imag();
            break;
        case IO_TYPE_COMPLEX_FIXPOINT:
            return this->fc.toComplexDouble();
            break;
        case IO_TYPE_BYTE:
            break;
    }
    return ComplexDouble(real, imag);
}


filter_io_t::~filter_io_t() {}

size_t filter_io_t::serialize(uint8_t *data) const
{
    size_t numBytes = 0;
    memcpy(data + numBytes, &type, sizeof(type));
    numBytes += sizeof(type);

    switch(type) {
        case IO_TYPE_COMPLEX_DOUBLE:
        {
            double value = rf.real();
            memcpy(data + numBytes, &value, sizeof(value));
            numBytes += sizeof(value);

            value = rf.imag();
            memcpy(data + numBytes, &value, sizeof(value));
            numBytes += sizeof(value);
            break;
        }
        case IO_TYPE_COMPLEX_FIXPOINT:
        {
            int64_t value = fc.real().to_int64();
            memcpy(data + numBytes, &value, sizeof(value));
            numBytes += sizeof(value);

            value = fc.imag().to_int64();
            memcpy(data + numBytes, &value, sizeof(value));
            numBytes += sizeof(value);
            break;
        }
        case IO_TYPE_BYTE:
        {
            memcpy(data + numBytes, &byte, sizeof(byte));
            numBytes += sizeof(byte);
            break;
        }
        case IO_TYPE_NULL:
            break;
    }

    return numBytes;
}

void filter_io_t::deserialize(const uint8_t *data, filter_io_t &output) const
{
    //TODO implement
}
