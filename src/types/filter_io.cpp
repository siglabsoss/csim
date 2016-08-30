#include <types/filter_io.hpp>

#include <iomanip>
std::ostream& operator<<(std::ostream& os, const filter_io_t& obj)
{
    switch(obj.type) {
        case IO_TYPE_COMPLEX_DOUBLE:
            os << obj.rf;
            break;
        case IO_TYPE_FIXED_COMPLEX_16_NEW:
            os << obj.fcn;
            break;
        case IO_TYPE_FIXED_COMPLEX_32_NEW:
            os << obj.fcn32;
            break;
        case IO_TYPE_FIXED_COMPLEX_64_NEW:
            os << obj.fcn64;
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

filter_io_t::filter_io_t(const filter_io_t &other)
{
    if (this != &other) {
        this->type = other.type;
        switch (other.type) {
            case IO_TYPE_COMPLEX_DOUBLE:
                this->rf = other.rf;
                break;
            case IO_TYPE_FIXED_COMPLEX_16_NEW:
                this->fcn = other.fcn;
                break;
            case IO_TYPE_FIXED_COMPLEX_32_NEW:
                this->fcn32 = other.fcn32;
                break;
            case IO_TYPE_FIXED_COMPLEX_64_NEW:
				this->fcn64 = other.fcn64;
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
            case IO_TYPE_FIXED_COMPLEX_16_NEW:
                this->fcn = rhs.fcn;
                break;
            case IO_TYPE_FIXED_COMPLEX_32_NEW:
                this->fcn32 = rhs.fcn32;
                break;
            case IO_TYPE_FIXED_COMPLEX_64_NEW:
                this->fcn64 = rhs.fcn64;
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

filter_io_t & filter_io_t::operator=(const FixedComplex16 &rhs)
{
    this->type = IO_TYPE_FIXED_COMPLEX_16_NEW;
    this->fcn = rhs;
    return *this;
}

filter_io_t & filter_io_t::operator=(const FixedComplex32 &rhs)
{
    this->type = IO_TYPE_FIXED_COMPLEX_32_NEW;
    this->fcn32 = rhs;
    return *this;
}

filter_io_t & filter_io_t::operator=(const FixedComplex64 &rhs)
{
    this->type = IO_TYPE_FIXED_COMPLEX_64_NEW;
    this->fcn64 = rhs;
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
        case IO_TYPE_FIXED_COMPLEX_16_NEW:
            real = this->fcn.real().to_double();
            imag = this->fcn.imag().to_double();
            break;
        case IO_TYPE_FIXED_COMPLEX_32_NEW:
            real = this->fcn32.real().to_double();
            imag = this->fcn32.imag().to_double();
            break;
        case IO_TYPE_FIXED_COMPLEX_64_NEW:
			real = this->fcn64.real().to_double();
			imag = this->fcn64.imag().to_double();
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
        case IO_TYPE_FIXED_COMPLEX_16_NEW:
        {
            double value = fcn.real().to_double();
            memcpy(data + numBytes, &value, sizeof(value));
            numBytes += sizeof(value);

            value = fcn.imag().to_double();
            memcpy(data + numBytes, &value, sizeof(value));
            numBytes += sizeof(value);
            break;
        }
        case IO_TYPE_FIXED_COMPLEX_32_NEW:
        {
            double value = fcn32.real().to_double();
            memcpy(data + numBytes, &value, sizeof(value));
            numBytes += sizeof(value);

            value = fcn32.imag().to_double();
            memcpy(data + numBytes, &value, sizeof(value));
            numBytes += sizeof(value);
            break;
        }
        case IO_TYPE_FIXED_COMPLEX_64_NEW:
		{
			double value = fcn64.real().to_double();
			memcpy(data + numBytes, &value, sizeof(value));
			numBytes += sizeof(value);

			value = fcn64.imag().to_double();
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
