#include <test/unit_test.hpp>

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>

#include <utils/utils.hpp> //reverseBits()
#include <filters/fixedfft.hpp>

using namespace std;


CSIM_TEST_SUITE_BEGIN(FixedFFT)

CSIM_TEST_CASE(FFT_OCTAVE)
{
    string infile("../csim/data/fft/input/data_file_complex1.csv");
       string answersfile("../csim/data/fft/answer/answers1.csv");
       cout << "program start" << endl;
       int i;
       int realInput[32769] = {0}; // default values
       int imagInput[32769] = {0};
       ifstream in(infile.c_str());
       BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not open data/fft/input/input3.txt");
       ifstream ans(answersfile.c_str());
       BOOST_REQUIRE_MESSAGE(ans.is_open(), "Could not open ../csim/data/fft/answers/answers1.csv");

       std::string token;
       string line;
       int inputs = 0;
       while(getline(in,line)) {

           istringstream ss(line);
           getline(ss, token, ',');
           stringstream strValue;
           strValue << token;
           int intValue;
           strValue >> intValue;
           realInput[inputs] = intValue;
           getline(ss, token, ',');
           stringstream strValue2;
           strValue2 << token;
           strValue2 >> intValue;
           imagInput[inputs++] = intValue;

       }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n
       int numAnswers = 0;
       FixedComplex<32> trueAnswers[32768];
       while(getline(ans,line)) {

                 istringstream ss(line);
                 getline(ss, token, ',');
                 stringstream strValue;
                 strValue << token;
                 int intValue;
                 strValue >> intValue;
                 trueAnswers[numAnswers].real = intValue;
                 getline(ss, token, ',');
                 stringstream strValue2;
                 strValue2 << token;
                 strValue2 >> intValue;
                trueAnswers[numAnswers++].imag = intValue;

             }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n


       FixedComplex<32> answers[32769];//Array to store answers
       int count = 0; //How many outputs have been collected

       int points = inputs;
       filter_io_t data;
       data.type =  IO_TYPE_FIXED_COMPLEX_32;
       fixedfft fft(points); //8 point fft

       for (int i = 0; i < 2; i++) {
           for (int j = 0; j < points; j++) {

               data.fc32 = FixedComplex<32>(realInput[j],imagInput[j]);
               fft.input(data);
               bool test = fft.output(data);
               if (test) {
                   answers[count++] = data.fc32;
               }
           }
       }
       cout << "Count is: " << count << endl << endl;

       assert(count == inputs);
       // If you want bits to be reversed

        cout << "Hopefully correct:" << endl;
        FixedComplex<32> temp[32769];
        for (i = 0; i < inputs; i++) {
            temp[reverseBits(inputs, i)] = answers[i];
        }//Reformats data in correct order


        for (i = 0; i < inputs; i++) {
            BOOST_CHECK_MESSAGE(abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) < .1 || abs(abs(temp[i].real) - abs(trueAnswers[i].real)) < 100 , "I: " << i << " " << temp[i].real << " " << trueAnswers[i].real );
            BOOST_CHECK_MESSAGE(abs((trueAnswers[i].imag - temp[i].imag )/(float)trueAnswers[i].imag) < .1 || abs(abs(temp[i].real) - abs(trueAnswers[i].real)) < 100, "I: " << i << " " << temp[i].imag << " " << trueAnswers[i].imag );
//            BOOST_CHECK_MESSAGE(abs(temp[i].real - trueAnswers[i].real) < 10000, "I: " << i << " " << temp[i].real << " " << trueAnswers[i].real );
//            BOOST_CHECK_MESSAGE(abs(trueAnswers[i].imag - temp[i].imag ) < 10000, "I: " << i << " " << temp[i].imag << " " << trueAnswers[i].imag );
 //           cout << abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) << endl;

        }


}

CSIM_TEST_SUITE_END()

