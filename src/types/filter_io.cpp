#include <types/filter_io.hpp>

#include <iomanip>

ComplexInt::ComplexInt() :
    exp(0)
{

}
/**
 * Extract the underlying bits from the fixed complex type. Use the most signicant bits of the
 * integer by shifting the underlying fixed complex bits appropriately. Set the scale exponent
 * based on the number of fractional bits.
 */
ComplexInt & ComplexInt::operator=(const FixedComplex &rhs)
{
    size_t realWordLength = rhs.real().wl();
    size_t imagWordLength = rhs.imag().wl();

    assert(realWordLength == imagWordLength);
    assert(realWordLength <= 32);

    size_t fracLen = realWordLength - rhs.real().iwl();

    int32_t realInt = rhs.real().range().to_int();
    int32_t imagInt = rhs.imag().range().to_int();

    realInt <<= (32 - realWordLength);
    imagInt <<= (32 - imagWordLength);
    this->c.real(realInt);
    this->c.imag(imagInt);
    this->exp = 31 - fracLen; //XXX double check sign

    return *this;
}

/**
 * Extract the underlying bits from the fixed complex type, allowing the user to set the
 * scaling exponent.
 */
void ComplexInt::assignFixedComplexWithExp(const FixedComplex &val, ssize_t exp)
{
    *this = val;
    this->exp = exp;
}

std::ostream& operator<<(std::ostream& os, const ComplexInt& obj)
{
    os << obj.c << " (2^" << obj.exp << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const filter_io_t& obj)
{
    switch(obj.type) {
        case IO_TYPE_COMPLEX_DOUBLE:
            os << obj.rf;
            break;
        case IO_TYPE_FIXED_COMPLEX:
            os << obj.fc;
            break;
        case IO_TYPE_INT32_COMPLEX:
            os << obj.intc;
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
    type(IO_TYPE_NULL),
    fc(sc_fix(0.0, 128, 64, SC_RND, SC_SAT), sc_fix(0.0, 128, 64, SC_RND, SC_SAT))
{}

filter_io_t::filter_io_t(const filter_io_t &other) :
        type(other.type),
        fc(sc_fix(0.0, 128, 64, SC_RND, SC_SAT), sc_fix(0.0, 128, 64, SC_RND, SC_SAT))
{
    if (this != &other) {
        switch (other.type) {
            case IO_TYPE_COMPLEX_DOUBLE:
                this->rf = other.rf;
                break;
            case IO_TYPE_FIXED_COMPLEX:
                this->fc = other.fc;
                break;
            case IO_TYPE_INT32_COMPLEX:
                this->intc = other.intc;
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
            case IO_TYPE_FIXED_COMPLEX:
                this->fc = rhs.fc;
                break;
            case IO_TYPE_INT32_COMPLEX:
                this->intc = rhs.intc;
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

filter_io_t & filter_io_t::operator=(const FixedComplex &rhs)
{
    this->type = IO_TYPE_INT32_COMPLEX;
    this->intc = rhs;
    return *this;
}

filter_io_t & filter_io_t::operator=(const uint8_t &rhs)
{
    this->type = IO_TYPE_BYTE;
    this->byte = rhs;
    return *this;
}

filter_io_t & filter_io_t::operator=(const ComplexInt &rhs)
{
    this->type = IO_TYPE_INT32_COMPLEX;
    this->intc = rhs;
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
        case IO_TYPE_FIXED_COMPLEX:
            real = this->fc.real().to_double();
            imag = this->fc.imag().to_double();
            break;
        case IO_TYPE_INT32_COMPLEX:
            return this->intc.toComplexDouble();
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
        case IO_TYPE_FIXED_COMPLEX:
        {
            double value = fc.real().to_double();
            memcpy(data + numBytes, &value, sizeof(value));
            numBytes += sizeof(value);

            value = fc.imag().to_double();
            memcpy(data + numBytes, &value, sizeof(value));
            numBytes += sizeof(value);
            break;
        }
        case IO_TYPE_INT32_COMPLEX:
        {
            int64_t value = rf.real();
            memcpy(data + numBytes, &value, sizeof(value));
            numBytes += sizeof(value);

            value = rf.imag();
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
