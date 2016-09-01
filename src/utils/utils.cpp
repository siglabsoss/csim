#include <cmath>
#include <utils/utils.hpp>

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

std::vector<FixedComplex16> complexRead16Scaled(std::string inFile)
{
	ifstream in(inFile.c_str());
	std::vector<FixedComplex16> input;
	typedef boost::tokenizer<boost::escaped_list_separator<char> > tokenizer;
	std::vector<std::string> vec;
	std::string line;
	FixedComplex16 temp;
	while (getline(in, line)) {
		tokenizer tok(line);
		vec.assign(tok.begin(), tok.end());
		temp.real(atof(vec[0].c_str())/32768.0);
		if (vec.size() == 2) {
		temp.imag(atof(vec[1].c_str())/32768.0);
		}
		else {
			temp.imag(0);
		}
		input.push_back(temp);

	} //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.
	in.close();
	return input;
}//For reading from complex file scaled by 32768. Returns a std::vector of FixedComplex16

std::vector<FixedComplex16> complexRead16Unscaled(std::string inFile)
{
	ifstream in(inFile.c_str());
	std::vector<FixedComplex16> input;
	typedef boost::tokenizer<boost::escaped_list_separator<char> > tokenizer;
	std::vector<std::string> vec;
	std::string line;
	FixedComplex16 temp;
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
	} //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.
	in.close();
	return input;
}//For reading from complex file with values between -1 and 1. Returns a std::vector of FixedComplex16

void print(FixedComplex32 x)
{
	cout << (x.real().range().to_int64()) << " " << (x.imag().range().to_int64()) << endl;
	cout << flush;
}//To print the value of a single FixedComplex32

void print(std::vector<FixedComplex32> x, int begin, int end)
{
	if (end == 0) {
		end = x.size();
	}
	for (int i = begin; i < end; i++) {
		cout << (x[i].real().range().to_int64()) << " " << (x[i].imag().range().to_int64()) << endl;
	}
	cout << flush;
}// To print the values of a std::vector of FixedComplex32


std::vector<FixedComplex32> complexRead32Scaled(std::string inFile)
{
	ifstream in(inFile.c_str());
	std::vector<FixedComplex32> input;
	typedef boost::tokenizer<boost::escaped_list_separator<char> > tokenizer;
	std::vector<std::string> vec;
	std::string line;
	FixedComplex32 temp;
	while (getline(in, line)) {
		tokenizer tok(line);
		vec.assign(tok.begin(), tok.end());
		temp.real(atof(vec[0].c_str())/32768.0);
		if (vec.size() == 2) {
		temp.imag(atof(vec[1].c_str())/32768.0);
		}
		else {
			temp.imag(0);
		}
		input.push_back(temp);
	} //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.
	in.close();
	return input;
}//For reading from complex file scaled by 32768. Returns a std::vector of FixedComplex32


std::vector<FixedComplex32> complexRead32Unscaled(std::string inFile)
{
	ifstream in(inFile.c_str());
	std::vector<FixedComplex32> input;
	typedef boost::tokenizer<boost::escaped_list_separator<char> > tokenizer;
	std::vector<std::string> vec;
	std::string line;
	FixedComplex32 temp;
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
	} //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.
	in.close();
	return input;
}//For reading from complex file scaled by 32768. Returns a std::vector of FixedComplex32


std::vector<FixedComplex64> complexRead64Scaled(std::string inFile)
{
	ifstream in(inFile.c_str());
	std::vector<FixedComplex64> input;
	typedef boost::tokenizer<boost::escaped_list_separator<char> > tokenizer;
	std::vector<std::string> vec;
	std::string line;
	FixedComplex64 temp;
	while (getline(in, line)) {
		tokenizer tok(line);
		vec.assign(tok.begin(), tok.end());
		temp.real(atof(vec[0].c_str())/32768.0);
		if (vec.size() == 2) {
		temp.imag(atof(vec[1].c_str())/32768.0);
		}
		else {
			temp.imag(0);
		}
		input.push_back(temp);
	} //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.
	in.close();
	return input;
}//For reading from complex file scaled by 32768. Returns a std::vector of FixedComplex32


std::vector<FixedComplex64> complexRead64Unscaled(std::string inFile)
{
	ifstream in(inFile.c_str());
	std::vector<FixedComplex64> input;
	typedef boost::tokenizer<boost::escaped_list_separator<char> > tokenizer;
	std::vector<std::string> vec;
	std::string line;
	FixedComplex64 temp;
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
	} //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.
	in.close();
	return input;
}//For reading from complex file scaled by 32768. Returns a std::vector of FixedComplex32

//XXX wrap entire file in namespace
namespace utils
{

std::unique_ptr<sc_fix> createDynamicFixedPoint(double val, size_t bitWidth, size_t &shiftBits)
{
    shiftBits = getShiftAmount(val);
    size_t intBits = getIntegerBits(val);
    assert(intBits <= bitWidth);
    if (shiftBits > 0) {
        val *= (1 << shiftBits);
    }
    return std::unique_ptr<sc_fix>(new sc_fix(val, bitWidth, intBits, SC_RND, SC_WRAP));
}

unsigned getShiftAmount(double coeff)
{
    int n = 0;
    coeff = abs(coeff); //we want same result for + and -
    if (coeff < 1) {
        unsigned ratio = static_cast<unsigned>(1.0 / coeff) >> 1;
        while (ratio) {
            n++;
            ratio >>= 1;
        }
    }
    return n;
}
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

};
