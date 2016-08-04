#pragma once

#include <interfaces/abstract_siso.hpp>
#include <types/fixedcomplex.hpp>
#include <memory>

#include <cstdint>


enum io_type_t : uint8_t {
    IO_TYPE_NULL = 0,
    IO_TYPE_COMPLEX_DOUBLE,
    IO_TYPE_FIXED_COMPLEX_16,
    IO_TYPE_FIXED_COMPLEX_32,
    IO_TYPE_FIXED_COMPLEX_16_NEW,
    IO_TYPE_FIXED_COMPLEX_32_NEW,
    IO_TYPE_BYTE
};
struct filter_io_t
{
    //Data members
    io_type_t type;
    FixedComplex<16> fc;
    FixedComplex<32> fc32;
    FixedComplex2<16, 1> fcn;
    FixedComplex2<32, 1> fcn32;
    union {
        std::complex<double> rf;
        uint8_t byte;
    };

    ~filter_io_t() {}

    filter_io_t() :
        type(IO_TYPE_NULL)
    {}

    filter_io_t(const filter_io_t &other)
    {
        if (this != &other) {
            this->type = other.type;
            switch (other.type) {
                case IO_TYPE_COMPLEX_DOUBLE:
                    this->rf = other.rf;
                    break;
                case IO_TYPE_FIXED_COMPLEX_16:
                    this->fc = other.fc;
                    break;
                case IO_TYPE_FIXED_COMPLEX_32:
                    this->fc32 = other.fc32;
                    break;
                case IO_TYPE_FIXED_COMPLEX_16_NEW:
                    this->fcn = other.fcn;
                    break;
                case IO_TYPE_FIXED_COMPLEX_32_NEW:
                    this->fcn32 = other.fcn32;
                    break;
                case IO_TYPE_BYTE:
                    this->byte = other.byte;
                    break;
                case IO_TYPE_NULL:
                    break;
            }
        }
    }

    filter_io_t & operator=(const filter_io_t &rhs)
    {
        if (this != &rhs) {
            this->type = rhs.type;
            switch (rhs.type) {
                case IO_TYPE_COMPLEX_DOUBLE:
                    this->rf = rhs.rf;
                    break;
                case IO_TYPE_FIXED_COMPLEX_16:
                    this->fc = rhs.fc;
                    break;
                case IO_TYPE_FIXED_COMPLEX_32:
                    this->fc32 = rhs.fc32;
                    break;
                case IO_TYPE_FIXED_COMPLEX_16_NEW:
                    this->fcn = rhs.fcn;
                    break;
                case IO_TYPE_FIXED_COMPLEX_32_NEW:
                    this->fcn32 = rhs.fcn32;
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

    filter_io_t & operator=(const std::complex<double> &rhs)
    {
        this->type = IO_TYPE_COMPLEX_DOUBLE;
        this->rf = rhs;
        return *this;
    }

    filter_io_t & operator=(const FixedComplex<16> &rhs)
    {
        this->type = IO_TYPE_FIXED_COMPLEX_16;
        this->fc = rhs;
        return *this;
    }

    filter_io_t & operator=(const FixedComplex<32> &rhs)
    {
        this->type = IO_TYPE_FIXED_COMPLEX_32;
        this->fc32 = rhs;
        return *this;
    }

    filter_io_t & operator=(const FixedComplex2<16, 1> &rhs)
    {
        this->type = IO_TYPE_FIXED_COMPLEX_16_NEW;
        this->fcn = rhs;
        return *this;
    }

    filter_io_t & operator=(const FixedComplex2<32, 1> &rhs)
    {
        this->type = IO_TYPE_FIXED_COMPLEX_32_NEW;
        this->fcn32 = rhs;
        return *this;
    }

    filter_io_t & operator=(const uint8_t &rhs)
    {
        this->type = IO_TYPE_BYTE;
        this->byte = rhs;
        return *this;
    }

    size_t serialize(uint8_t *data) const
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
            case IO_TYPE_FIXED_COMPLEX_16:
            {
                int value = fc.real;
                memcpy(data + numBytes, &value, sizeof(value));
                numBytes += sizeof(value);

                value = fc.imag;
                memcpy(data + numBytes, &value, sizeof(value));
                numBytes += sizeof(value);
                break;
            }
            case IO_TYPE_FIXED_COMPLEX_32:
            {
               int value = fc32.real;
               memcpy(data + numBytes, &value, sizeof(value));
               numBytes += sizeof(value);

               value = fc32.imag;
               memcpy(data + numBytes, &value, sizeof(value));
               numBytes += sizeof(value);
               break;
           }
            case IO_TYPE_FIXED_COMPLEX_16_NEW:
                break; //XXX TODO
            case IO_TYPE_FIXED_COMPLEX_32_NEW:
                break; //XXX TODO
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

    void deserialize(const uint8_t *data, filter_io_t &output) const
    {
        //TODO implement
    }
};
extern std::ostream& operator<<(std::ostream& os, const filter_io_t& obj);

class FilterChainElement : public AbstractSISO< filter_io_t, filter_io_t >
{
public:
    virtual ~FilterChainElement() {}

    FilterChainElement(std::string name = std::string("NONAME"));

    friend FilterChainElement& operator+(const FilterChainElement &lhs, FilterChainElement &rhs)
    {
        rhs.m_next.reset((FilterChainElement *)&lhs);
        return rhs;
    }

    const std::string &getName() const;

    bool shouldPublish() const;

    void shouldPublish(bool desired);

    std::unique_ptr<FilterChainElement> m_next;
private:
    std::string         m_name;
    bool                m_shouldPublish;

    static unsigned int instanceCount;
};
