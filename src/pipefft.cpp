#include <iostream>
#include <stdlib.h>
#include "utils.h"
#include "fixedfft.h"
#include "floatfft.h"

using namespace std;


int main(void)
{

    string infile = "../csim/data/fft/input/input2.txt";
    string outfile = "../csim/data/fft/output/out2.csv"
    cout << "program start" << endl;
    int i;
//
//	{
//	floatfft fft(8);
//	for(i = 0; i < 3; i++)
//	{
//		fft.inputandtick(5);
//		fft.inputandtick(6);
//		fft.inputandtick(8);
//		fft.inputandtick(-5);
//		fft.inputandtick(6);
//		fft.inputandtick(12);
//		fft.inputandtick(10);
//		fft.inputandtick(9);
//	}
//	}

    int realInput[8] = {5,6,8,-5,6,12,10,9}; // default values
    int imagInput[8] = {0};
    ifstream in(infile);

    if (!in.is_open()){
            cout << "error reading" << endl;
            return 1;
    }//If cannot read from file, return 1;

    std::string token;
    for (i = 0; i < 8; i++) {
        string line;
        getline(in,line);
        istringstream ss(line);
        getline(ss, token, ',');
        stringstream strValue;
        strValue << token;
        int intValue;
        strValue >> intValue;
        realInput[i] = intValue;
        getline(ss, token, ',');
        stringstream strValue2;
        strValue2 << token;
        strValue2 >> intValue;
        imagInput[i] = intValue;
    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    int scale = 1;
    fixedfft fft(8);
    for (i = 0; i < 3; i++) {

        fft.inputandtick(FixedComplex<32>(realInput[0] * scale, imagInput[0] * scale));
        fft.inputandtick(FixedComplex<32>(realInput[1] * scale, imagInput[1] * scale));
        fft.inputandtick(FixedComplex<32>(realInput[2] * scale, imagInput[2] * scale));
        fft.inputandtick(FixedComplex<32>(realInput[3] * scale, imagInput[3] * scale));
        fft.inputandtick(FixedComplex<32>(realInput[4] * scale, imagInput[4] * scale));
        fft.inputandtick(FixedComplex<32>(realInput[5] * scale, imagInput[5] * scale));
        fft.inputandtick(FixedComplex<32>(realInput[6] * scale, imagInput[6] * scale));
        fft.inputandtick(FixedComplex<32>(realInput[7] * scale, imagInput[7] * scale));
    }

    ifstream in2(outfile);
    if (!in2.is_open())
         cout << "error reading" << endl;


     string str[8];
     string ans;
     for (i = 0; i < 8; i++) {
         in2 >> str[i];
     }//Reads in output file

     in2.close();

     cout << "Hopefully correct:" << endl;
     string temp[10];
     for (i = 0; i < 8; i++) {
         temp[reverseBits(8, i)] = str[i];
     }//Reformats data in correct order


     ofstream out2(outfile);

     for (i = 0; i < 8; i++) {
         out2 << temp[i] << endl;
         cout << temp[i] << endl;
     }//Prints data out in correct order



    //	floatfftstage stageone(8);
    //	floatfftstage stagetwo(4);
    //	floatfftstage stagethree(2);
    //	floatfftprint printer(8);
    //
    //	stageone.next = &stagetwo;
    //	stagetwo.next = &stagethree;
    //	stagethree.next = &printer;
    //	for(i = 0; i < 10; i++)
    //	{
    //		stageone.inputandtick(5);
    //		stageone.inputandtick(6);
    //		stageone.inputandtick(8);
    //		stageone.inputandtick(-5);
    //		stageone.inputandtick(6);
    //		stageone.inputandtick(12);
    //		stageone.inputandtick(10);
    //		stageone.inputandtick(9);
    //	}

    //	floatfftstage stageone(4);
    //	floatfftstage stagetwo(2);
    //	floatfftprint printer(8);
    //
    //	stageone.next = &stagetwo;
    //	stagetwo.next = &printer;
    //
    //	stageone.inputandtick(5);
    //	stageone.inputandtick(6);
    //	stageone.inputandtick(8);
    //	stageone.inputandtick(-5);
    //
    //
    //	stageone.inputandtick(5);
    //	stageone.inputandtick(6);
    //	stageone.inputandtick(8);
    //	stageone.inputandtick(-5);
    //
    //	stageone.inputandtick(5);
    //	stageone.inputandtick(6);
    //	stageone.inputandtick(8);
    //	stageone.inputandtick(-5);

    cout << "program end" << endl;

}

