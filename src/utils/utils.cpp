#include <cmath>
#include <utils/utils.hpp>

using namespace std;
using namespace boost;

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

vector<FixedComplex16> complexRead16Scaled(string inFile)
{
	ifstream in(inFile.c_str());
	vector<FixedComplex16> input;
	char ** ptr;
	typedef tokenizer<escaped_list_separator<char> > Tokenizer;
	vector<string> vec;
	string line;
	FixedComplex16 temp;
	while (getline(in, line)) {
		Tokenizer tok(line);
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
}//For reading from complex file scaled by 32768. Returns a vector of FixedComplex16

vector<FixedComplex16> complexRead16Unscaled(string inFile)
{
	ifstream in(inFile.c_str());
	vector<FixedComplex16> input;
	char ** ptr;
	typedef tokenizer<escaped_list_separator<char> > Tokenizer;
	vector<string> vec;
	string line;
	FixedComplex16 temp;
	while (getline(in, line)) {
		Tokenizer tok(line);
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
}//For reading from complex file with values between -1 and 1. Returns a vector of FixedComplex16

void print(FixedComplex32 x)
{
	cout << (x.real().range().to_int64()) << " " << (x.imag().range().to_int64()) << endl;
	cout << flush;
}//To print the value of a single FixedComplex32

void print(vector<FixedComplex32> x, int begin, int end)
{
	if (end == 0) {
		end = x.size();
	}
	for (int i = begin; i < end; i++) {
		cout << (x[i].real().range().to_int64()) << " " << (x[i].imag().range().to_int64()) << endl;
	}
	cout << flush;
}// To print the values of a vector of FixedComplex32


vector<FixedComplex32> complexRead32Scaled(string inFile)
{
	ifstream in(inFile.c_str());
	vector<FixedComplex32> input;
	char ** ptr;
	typedef tokenizer<escaped_list_separator<char> > Tokenizer;
	vector<string> vec;
	string line;
	FixedComplex32 temp;
	while (getline(in, line)) {
		Tokenizer tok(line);
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
}//For reading from complex file scaled by 32768. Returns a vector of FixedComplex32


vector<FixedComplex32> complexRead32Unscaled(string inFile)
{
	ifstream in(inFile.c_str());
	vector<FixedComplex32> input;
	char ** ptr;
	typedef tokenizer<escaped_list_separator<char> > Tokenizer;
	vector<string> vec;
	string line;
	FixedComplex32 temp;
	while (getline(in, line)) {
		Tokenizer tok(line);
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
}//For reading from complex file scaled by 32768. Returns a vector of FixedComplex32


vector<FixedComplex64> complexRead64Scaled(string inFile)
{
	ifstream in(inFile.c_str());
	vector<FixedComplex64> input;
	char ** ptr;
	typedef tokenizer<escaped_list_separator<char> > Tokenizer;
	vector<string> vec;
	string line;
	FixedComplex64 temp;
	while (getline(in, line)) {
		Tokenizer tok(line);
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
}//For reading from complex file scaled by 32768. Returns a vector of FixedComplex32


vector<FixedComplex64> complexRead64Unscaled(string inFile)
{
	ifstream in(inFile.c_str());
	vector<FixedComplex64> input;
	char ** ptr;
	typedef tokenizer<escaped_list_separator<char> > Tokenizer;
	vector<string> vec;
	string line;
	FixedComplex64 temp;
	while (getline(in, line)) {
		Tokenizer tok(line);
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
}//For reading from complex file scaled by 32768. Returns a vector of FixedComplex32

