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

       int i = 0;
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


       assert(count == inputs);
       // If you want bits to be reversed

        FixedComplex<32> temp[32769];
        for (i = 0; i < inputs; i++) {
            temp[reverseBits(inputs, i)] = answers[i];
        }//Reformats data in correct order


        for (i = 0; i < inputs; i++) {
            BOOST_CHECK_MESSAGE(abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) < .13 || abs(abs(temp[i].real) - abs(trueAnswers[i].real)) < 3000 ,
                    "I: " << i << " Output: " << temp[i].real << " Answer: " << trueAnswers[i].real << "Ratio: " << abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) );
            BOOST_CHECK_MESSAGE(abs((trueAnswers[i].imag - temp[i].imag )/(float)trueAnswers[i].imag) < .13 || abs(abs(temp[i].imag) - abs(trueAnswers[i].imag)) < 3000,
                    "I: " << i << " Output: " << temp[i].imag << " Answer: " << trueAnswers[i].imag << "Ratio: " << abs((temp[i].imag - trueAnswers[i].imag)/(float)trueAnswers[i].imag) );

        }


}


CSIM_TEST_CASE(FFT_TWO_INPUTS)
{
    string infile("../csim/data/fft/input/data_file_complex2.csv");
    string infile2("../csim/data/fft/input/data_file_complex3.csv");
    string answersfile("../csim/data/fft/answer/answers2.csv");
    string answersfile2("../csim/data/fft/answer/answers3.csv");

    int i = 0;
    int realInput[32769] = {0}; // default values
    int imagInput[32769] = {0};
    ifstream in(infile.c_str());
    ifstream in2(infile2.c_str());
    BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not open ../csim/data/fft/input/data_file_complex2.csv");
    BOOST_REQUIRE_MESSAGE(in2.is_open(), "Could not open ../csim/data/fft/input/data_file_complex3.csv");
    ifstream ans(answersfile.c_str());
    BOOST_REQUIRE_MESSAGE(ans.is_open(), "Could not open ../csim/data/fft/answers/answers2.csv");
    ifstream ans2(answersfile2.c_str());
    BOOST_REQUIRE_MESSAGE(ans2.is_open(), "Could not open ../csim/data/fft/answers/answers3.csv");

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

    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < points; j++) {

            data.fc32 = FixedComplex<32>(realInput[j],imagInput[j]);
            fft.input(data);
            bool test = fft.output(data);
            if (test) {
               answers[count++] = data.fc32;
            }
        }
    }

    while(getline(in2,line)) {

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



for (int j = 0; j < points; j++) {

           data.fc32 = FixedComplex<32>(0,0);
           fft.input(data);
           bool test = fft.output(data);
           if (test) {
               answers[count++] = data.fc32;
           }
}

    FixedComplex<32> temp[32769];
    for (i = 0; i < inputs; i++) {
        temp[reverseBits(inputs, i)] = answers[i];
    }//Reformats data in correct order


    for (i = 0; i < inputs; i++) {
        BOOST_CHECK_MESSAGE(abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) < .13 || abs(abs(temp[i].real) - abs(trueAnswers[i].real)) < 3000 ,
                "I: " << i << " Output: " << temp[i].real << " Answer: " << trueAnswers[i].real << "Ratio: " << abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) );
        BOOST_CHECK_MESSAGE(abs((trueAnswers[i].imag - temp[i].imag )/(float)trueAnswers[i].imag) < .13 || abs(abs(temp[i].imag) - abs(trueAnswers[i].imag)) < 3000,
                "I: " << i << " Output: " << temp[i].imag << " Answer: " << trueAnswers[i].imag << "Ratio: " << abs((temp[i].imag - trueAnswers[i].imag)/(float)trueAnswers[i].imag) );

        }


    inputs = 0;



    numAnswers = 0;

    while(getline(ans2,line)) {

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

    count = 0; //How many outputs have been collected
    data.type =  IO_TYPE_FIXED_COMPLEX_32;


    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < points; j++) {

            data.fc32 = FixedComplex<32>(realInput[j],imagInput[j]);
            fft.input(data);
            bool test = fft.output(data);
            if (test) {
                answers[count++] = data.fc32;
            }
        }
    }
    for (int j = 0; j < points; j++) {

               data.fc32 = FixedComplex<32>(0,0);
               fft.input(data);
               bool test = fft.output(data);
               if (test) {
                   answers[count++] = data.fc32;
               }
    }


    for (i = 0; i < inputs; i++) {
        temp[reverseBits(inputs, i)] = answers[i];
    }//Reformats data in correct order


    for (i = 0; i < inputs; i++) {
        BOOST_CHECK_MESSAGE(abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) < .13 || abs(abs(temp[i].real) - abs(trueAnswers[i].real)) < 3000 ,
           "I: " << i << " Output: " << temp[i].real << " Answer: " << trueAnswers[i].real << "Ratio: " << abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) );
        BOOST_CHECK_MESSAGE(abs((trueAnswers[i].imag - temp[i].imag )/(float)trueAnswers[i].imag) < .13 || abs(abs(temp[i].imag) - abs(trueAnswers[i].imag)) < 3000,
           "I: " << i << " Output: " << temp[i].imag << " Answer: " << trueAnswers[i].imag << "Ratio: " << abs((temp[i].imag - trueAnswers[i].imag)/(float)trueAnswers[i].imag) );

    }
}//Checks for two consecutive sets of inputs in the same FFT.


//CSIM_TEST_CASE(FFT_VERILOG)
//{
//       string infile("../csim/data/fft/output/out1.txt");
//       string answersfile("../csim/data/fft/input/verilog.txt");
//
//       int i = 0;
//       FixedComplex<32> bitReversedOutput[32768];
//       ifstream in(infile.c_str());
//       BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not open data/fft/input/verilog.txt");
//       ifstream ans(answersfile.c_str());
//       BOOST_REQUIRE_MESSAGE(ans.is_open(), "Could not open ../csim/data/fft/output/out1.txt");
//
//       std::string token;
//       string line;
//       int inputs = 0;
//       while(getline(in,line)) {
//           istringstream ss(line);
//           getline(ss, token, ',');
//           stringstream strValue;
//           strValue << token;
//           int intValue;
//           strValue >> intValue;
//           bitReversedOutput[inputs].real = intValue;
//           getline(ss, token, ',');
//           stringstream strValue2;
//           strValue2 << token;
//           strValue2 >> intValue;
//           bitReversedOutput[inputs++].imag = intValue;
//       }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n
//
//       int numAnswers = 0;
//       FixedComplex<32> trueAnswers[32768];
//       while(getline(ans,line)) {
//
//         istringstream ss(line);
//         getline(ss, token, ',');
//         stringstream strValue;
//         strValue << token;
//         int intValue;
//         strValue >> intValue;
//         trueAnswers[numAnswers].real = intValue;
//         getline(ss, token, ',');
//         stringstream strValue2;
//         strValue2 << token;
//         strValue2 >> intValue;
//        trueAnswers[numAnswers++].imag = intValue;
//
//     }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n
//
////
////        FixedComplex<32> temp[32769];
////        for (i = 0; i < inputs; i++) {
////            temp[i] = bitReversedOutput[reverseBits(inputs, i)];
////        }//Reformats data in correct order
//
//
//       FixedComplex<32> temp[32769];
//       for (i = 0; i < inputs; i++) {
//           temp[i] = bitReversedOutput[i];
//       }
//
//
//
//        for (i = 0; i < 10; i++) {
//            BOOST_CHECK_MESSAGE(abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) <  .01 || abs(abs(temp[i].real) - abs(trueAnswers[i].real)) < 10 ,
//                    "I: " << i << " Output: " << temp[i].real << " Answer: " << trueAnswers[i].real << " Ratio: " << abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) );
//            BOOST_CHECK_MESSAGE(abs((trueAnswers[i].imag - temp[i].imag )/(float)trueAnswers[i].imag) < .01 || abs(abs(temp[i].imag) - abs(trueAnswers[i].imag)) < 10,
//                    "I: " << i << " Output: " << temp[i].imag << " Answer: " << trueAnswers[i].imag << " Ratio: " << abs((temp[i].imag - trueAnswers[i].imag)/(float)trueAnswers[i].imag) );
//
//        }
//
//
//}//Used for comparing percent error of two files for the FFT.


CSIM_TEST_SUITE_END()

