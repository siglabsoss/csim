#include <cmath>
#include <utils/utils.hpp>

unsigned int reverseBits(int N, unsigned int num)
{
//    unsigned int  NO_OF_BITS = sizeof(num) * 8;
    unsigned int NO_OF_BITS = log2(N);
    unsigned int reverse_num = 0;
    int i;
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


std::vector<FixedComplex16> complexRead16Scaled(std::string inFile)
{
	ifstream in(inFile.c_str());
	std::vector<FixedComplex16> input;
	char ** ptr;
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
	char ** ptr;
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
	char ** ptr;
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
	char ** ptr;
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
	char ** ptr;
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
	char ** ptr;
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


