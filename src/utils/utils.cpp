#include <cmath>
#include <utils/utils.hpp>
#include <types/complexdouble.hpp>

unsigned int reverseBits(int N, unsigned int num)
{
//    unsigned int  NO_OF_BITS = sizeof(num) * 8;
    unsigned int NO_OF_BITS = log2(N);
    unsigned int reverse_num = 0;
    unsigned int i;
    for (i = 0; i < NO_OF_BITS; i++) {
        if ((num & (1 << i)))
            reverse_num |= 1 << ((NO_OF_BITS - 1) - i);
    }
    return reverse_num;
}

double bound(double min, double max, double val)
{
    if (val > max) {
        val = max;
    } else if (val < min) {
        val = min;
    }
    return val;
}

unsigned int calculateHammingDistance(uint8_t a, uint8_t b)
{
    unsigned int count = 0;
    uint8_t diff = a ^ b;
    for (unsigned i = 0; i < 8; i++) {
        if (diff & (1 << i)) {
            count++;
        }
    }
    return count;
}

template <typename T>
std::vector<T> readComplexFromCSV(const std::string &inFile, double scaleDownFactor)
{
    ifstream in(inFile.c_str());
    std::vector<T> input;
    typedef boost::tokenizer<boost::escaped_list_separator<char> > tokenizer;
    std::vector<std::string> vec;
    std::string line;
    T temp;
    while (getline(in, line)) {
        tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        temp.real(atof(vec[0].c_str()) / scaleDownFactor);
        if (vec.size() == 2) {
            temp.imag(atof(vec[1].c_str()) / scaleDownFactor);
        }
        else {
            temp.imag(0);
        }
        input.push_back(temp);
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.
    in.close();
    return input;
}
template std::vector<ComplexDouble>  readComplexFromCSV(const std::string &inFile, double scaleDownFactor);
template std::vector<FixedComplex16> readComplexFromCSV(const std::string &inFile, double scaleDownFactor);
template std::vector<FixedComplex32> readComplexFromCSV(const std::string &inFile, double scaleDownFactor);
template std::vector<FixedComplex2_30> readComplexFromCSV(const std::string &inFile, double scaleDownFactor);
template std::vector<FixedComplex64> readComplexFromCSV(const std::string &inFile, double scaleDownFactor);

//XXX wrap entire file in namespace
namespace utils
{

std::unique_ptr<sc_fix> createDynamicFixedPoint(double val, size_t bitWidth, ssize_t &shiftBits)
{
    shiftBits = -getShiftAmount(val);
    size_t intBits = getIntegerBits(val);
    assert(intBits <= bitWidth);
    if (shiftBits > 0) {
        val *= (1 << shiftBits);
    } else {
        shiftBits = 0; //we only consider scaling up, not down
    }
    return std::unique_ptr<sc_fix>(new sc_fix(val, bitWidth, intBits, SC_RND, SC_WRAP));
}

/**
 * Calculates the amount to shift a fixed point value such that there are no leading zeros
 * in the fractional component, or in the case of values above one, how much to scale down
 * in powers of two to normalize the value.
 *
 * @return Amount to shift. Left is negative.
 */
int getShiftAmount(double coeff)
{
    int n = 0;
    if (coeff == 0.0) {
        return n;
    }
    coeff = abs(coeff); //we want same result for + and -
    if (coeff < 1) {
        unsigned ratio = static_cast<unsigned>(1.0 / coeff) >> 2;
        while (ratio) {
            n--;
            ratio >>= 1;
        }
    } else {
        unsigned ratio = static_cast<unsigned>(coeff / 1.0);
        while (ratio) {
            n++;
            ratio >>= 1;
        }
    }
    return n;
}

/**
 * Calculates the number of bits required to store the non-fractional component of a signed,
 * fixed-point integer.
 *
 * @return The number of bits required to store the non-fractional component of a signed, fixed-point integer.
 */
unsigned getIntegerBits(double coeff)
{
    unsigned int_coeff = abs(static_cast<int>(coeff));
    unsigned n = 1; //one sign bit at a minimum
    while (int_coeff) {
        n++;
        int_coeff >>= 1;
    }
    return n;
}

bool addition32DoesOverflow(int32_t a, int32_t b)
{
    //Consider overflow if signs were the same and then sign of result changed
    bool sameSign = (a >= 0 && b >= 0) || (a < 0 && b < 0);
    if (sameSign) {
        int32_t sum = a + b;
        if (a >= 0) {
            if (sum < 0) {
                return true;
            }
        } else {
            if (sum >= 0) {
                return true;
            }
        }
    }

    return false;
}

size_t calculateInt32ScaleExponent(const std::vector<ComplexDouble> &values)
{
    double max = 0.0;
    for (size_t i = 0; i < values.size(); i++) {
        if (abs(values[i].real()) > max) {
            max = abs(values[i].real());
        }
        if (abs(values[i].imag()) > max) {
            max = abs(values[i].imag());
        }
    }
    size_t result = 0;
    if (max == 0.0) {
        return result;
    }

    return 31 + getShiftAmount(max);
}

void complexScalarMultiply(FixedComplex &result, const FixedComplex &complex, const sc_dt::sc_fix &scalar)
{
    //For some reason not yet known, when dealing with complex<sc_fix> we must store the calculation in
    //intermediate results instead of acting on the complex variable directly. This does not seem to be necessary
    //when dealing with sc_fixed<N,M>.
    sc_dt::sc_fix tempResultReal(result.real().wl(), result.real().iwl());
    sc_dt::sc_fix tempResultImag(result.imag().wl(), result.imag().iwl());

    tempResultReal = complex.real() * scalar;
    tempResultImag = complex.imag() * scalar;

    result.real(tempResultReal);
    result.imag(tempResultImag);
}

void complexScalarMultiplyAccumulate(FixedComplex &accum, const FixedComplex &complex, const sc_dt::sc_fix &scalar)
{
    //For some reason not yet known, when dealing with complex<sc_fix> we must store the calculation in
    //intermediate results instead of acting on the complex variable directly. This does not seem to be necessary
    //when dealing with sc_fixed<N,M>.
    sc_dt::sc_fix tempResultReal(accum.real().wl(), accum.real().iwl());
    sc_dt::sc_fix tempResultImag(accum.imag().wl(), accum.imag().iwl());

    tempResultReal = complex.real() * scalar;
    tempResultImag = complex.imag() * scalar;

    tempResultReal += accum.real();
    tempResultImag += accum.imag();

    accum.real(tempResultReal);
    accum.imag(tempResultImag);
}

};
