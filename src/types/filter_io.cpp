#include <types/filter_io.hpp>

#include <iomanip>
#include <cassert>
#include <cstring>

std::ostream & operator<<(std::ostream& os, const filter_io_t& obj)
{
    switch (obj.type) {
    case IO_TYPE_COMPLEX_DOUBLE:
        os << obj.rf;
        break;

    case IO_TYPE_COMPLEX_FIXPOINT:
        os << obj.fc;
        break;

    case IO_TYPE_FIXPOINT:
        os << obj.fp;
        break;

    case IO_TYPE_BYTE:
        os << "[0x" << std::hex << std::setw(2) << std::setfill('0') <<
        static_cast<int>(obj.byte) << "]";
        break;

    case IO_TYPE_BIT:
        os << "[0b" << static_cast<int>(obj.bit) << "]";
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


filter_io_t::filter_io_t(const ComplexDouble in) :
		type(IO_TYPE_COMPLEX_DOUBLE)
{
	this->rf = in;
}


filter_io_t::filter_io_t(const uint8_t in) :
        type(IO_TYPE_BYTE)
{
	this->byte = in;
}

filter_io_t::filter_io_t(const SLFixComplex in) :
		type(IO_TYPE_COMPLEX_FIXPOINT)
{
	this->fc.setFormat(in.getFormat());
	this->fc = in;
}


filter_io_t::filter_io_t(const filter_io_t& other) :
    type(other.type)
{
    if (this != &other) {
        switch (other.type) {
        case IO_TYPE_COMPLEX_DOUBLE:
            this->rf = other.rf;
            break;

        case IO_TYPE_COMPLEX_FIXPOINT:
            this->fc.setFormat(other.fc.getFormat());
            this->fc = other.fc;
            break;

        case IO_TYPE_FIXPOINT:
            this->fp.setFormat(other.fp.getFormat());
            this->fp = other.fp;
            break;

        case IO_TYPE_BYTE:
            this->byte = other.byte;
            break;

        case IO_TYPE_BIT:
            this->bit = other.bit;
            break;

        case IO_TYPE_NULL:
            break;
        }
    }
}

filter_io_t& filter_io_t::operator=(const filter_io_t& rhs)
{
    if (this != &rhs) {
        this->type = rhs.type;

        switch (rhs.type) {
        case IO_TYPE_COMPLEX_DOUBLE:
            this->rf = rhs.rf;
            break;

        case IO_TYPE_COMPLEX_FIXPOINT:
            this->fc.setFormat(rhs.fc.getFormat());
            this->fc = rhs.fc;
            break;

        case IO_TYPE_FIXPOINT:
            this->fp.setFormat(rhs.fp.getFormat());
            this->fp = rhs.fp;
            break;

        case IO_TYPE_BYTE:
            this->byte = rhs.byte;
            break;

        case IO_TYPE_BIT:
            this->bit = rhs.bit;
            break;

        case IO_TYPE_NULL:
            break;
        }
    }
    return *this;
}

filter_io_t& filter_io_t::operator=(const ComplexDouble& rhs)
{
    this->type = IO_TYPE_COMPLEX_DOUBLE;
    this->rf   = rhs;
    return *this;
}

filter_io_t& filter_io_t::operator=(const SLFixComplex& rhs)
{
    this->type = IO_TYPE_COMPLEX_FIXPOINT;
    this->fc.setFormat(rhs.getFormat());
    this->fc = rhs;
    return *this;
}

filter_io_t& filter_io_t::operator=(const SLFixPoint& rhs)
{
    this->type = IO_TYPE_FIXPOINT;
    this->fp.setFormat(rhs.getFormat());
    this->fp = rhs;
    return *this;
}

filter_io_t& filter_io_t::operator=(const uint8_t& rhs)
{
    this->type = IO_TYPE_BYTE;
    this->byte = rhs;
    return *this;
}

filter_io_t& filter_io_t::operator=(const bool& rhs)
{
    this->type = IO_TYPE_BIT;
    this->bit  = rhs;
    return *this;
}

ComplexDouble filter_io_t::toComplexDouble() const
{
    double real = 0, imag = 0;

    switch (this->type) {
    case IO_TYPE_NULL:
        break;

    case IO_TYPE_COMPLEX_DOUBLE:
        real = this->rf.real();
        imag = this->rf.imag();
        break;

    case IO_TYPE_COMPLEX_FIXPOINT:
        return this->fc.toComplexDouble();

        break;

    case IO_TYPE_FIXPOINT:
        real = this->fp.to_double();
        imag = 0.0;
        break;

    case IO_TYPE_BYTE:
        break;

    case IO_TYPE_BIT:
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

    switch (type) {
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

    case IO_TYPE_FIXPOINT:
    {
        int64_t value = fp.to_int64();
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

    case IO_TYPE_BIT:
    {
        memcpy(data + numBytes, &bit, sizeof(bit));
        numBytes += sizeof(bit);
        break;
    }

    case IO_TYPE_NULL:
        break;
    }

    return numBytes;
}

void filter_io_t::deserialize(const uint8_t *data, filter_io_t& output) const
{
    // TODO implement
}
