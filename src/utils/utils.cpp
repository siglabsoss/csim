#include <cmath>
#include <utils/utils.hpp>
#include <types/complexdouble.hpp>


namespace utils {
unsigned int reverseBits(int N, unsigned int num)
{
    //    unsigned int  NO_OF_BITS = sizeof(num) * 8;
    unsigned int NO_OF_BITS  = log2(N);
    unsigned int reverse_num = 0;
    unsigned int i;

    for (i = 0; i < NO_OF_BITS; i++) {
        if ((num & (1 << i))) reverse_num |= 1 << ((NO_OF_BITS - 1) - i);
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
    uint8_t diff       = a ^ b;

    for (unsigned i = 0; i < 8; i++) {
        if (diff & (1 << i)) {
            count++;
        }
    }
    return count;
}

template<typename T>
std::vector<T>readComplexFromCSV(const std::string& inFile)
{
    std::ifstream in(inFile.c_str());

    std::vector<T> input;
    typedef boost::tokenizer<boost::escaped_list_separator<char> >tokenizer;
    std::vector<std::string> vec;
    std::string line;
    T temp;

    while (getline(in, line)) {
        tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        temp.real(atof(vec[0].c_str()));

        if (vec.size() == 2) {
            temp.imag(atof(vec[1].c_str()));
        }
        else {
            temp.imag(0);
        }
        input.push_back(temp);
    } // Gets each line of data. Stores real and imaginary parts separate in
      // FixedComplex. i stores total number of inputs.
    in.close();
    return input;
}

template std::vector<ComplexDouble>readComplexFromCSV(const std::string& inFile);

template<typename T>
std::vector<std::pair<T, T> >      readComplexPairFromCSV(
    const std::string& inFile)
{
    std::ifstream in(inFile.c_str());

    std::vector<std::pair<T, T> > input;
    typedef boost::tokenizer<boost::escaped_list_separator<char> >tokenizer;
    std::vector<std::string> vec;
    std::string line;
    T tempA;
    T tempB;

    while (getline(in, line)) {
        tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());

        if (vec.size() == 2) { // real only values
            tempA.real(atof(vec[0].c_str()));
            tempB.real(atof(vec[1].c_str()));
        } else if (vec.size() == 4) {
            tempA.real(atof(vec[0].c_str()));
            tempA.imag(atof(vec[1].c_str()));
            tempB.real(atof(vec[2].c_str()));
            tempB.imag(atof(vec[3].c_str()));
        } else {
            assert(true); // bad input
        }
        input.push_back(std::pair<T, T>(tempA, tempB));
    } // Gets each line of data. Stores real and imaginary parts separate in
      // FixedComplex. i stores total number of inputs.
    in.close();
    return input;
}

template std::vector<std::pair<ComplexDouble,
                               ComplexDouble> >readComplexPairFromCSV(
    const std::string& inFile);

/**
 * Calculates the amount to shift a fixed point value such that there are no
 ******leading zeros
 * in the fractional component, or in the case of values above one, how much to
 ******scale down
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
    coeff = fabs(coeff); // we want same result for + and -

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
 * Calculates the number of bits required to store the non-fractional component
 ******of a signed,
 * fixed-point integer.
 *
 * @return The number of bits required to store the non-fractional component of
 ******a signed, fixed-point integer.
 */
unsigned getIntegerBits(double coeff)
{
    unsigned int_coeff = abs(static_cast<int>(coeff));
    unsigned n         = 1; // one sign bit at a minimum

    while (int_coeff) {
        n++;
        int_coeff >>= 1;
    }
    return n;
}

bool addition32DoesOverflow(int32_t a, int32_t b)
{
    // Consider overflow if signs were the same and then sign of result changed
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

size_t calculateInt32ScaleExponent(const std::vector<ComplexDouble>& values)
{
    double max = 0.0;

    for (size_t i = 0; i < values.size(); i++) {
        double real = fabs(values[i].real());
        double imag = fabs(values[i].imag());

        if (real > max) {
            max = real;
        }

        if (imag > max) {
            max = imag;
        }
    }
            assert(max > 0.0);
    return 31 + getShiftAmount(max);
}

double angleDiff(double a, double b)
{
    double result = a - b;

    if (result > M_PI) {
        result = 2 * M_PI - result;
    }
    return result;
}
};
