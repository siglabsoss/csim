#include <test/unit_test.hpp>
#include <filters/fixed_fir.hpp>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <utils/utils.hpp>
using namespace boost;
using namespace std;

CSIM_TEST_SUITE_BEGIN(FIRFilter)

CSIM_TEST_CASE(REAL_FILTER)
{
    vector<FixedComplex16> input; //Vector to hold inputs
    vector<FixedComplex16> output; //Vector to hold outputs
    vector<FixedComplex16> answers;
    string data("./data/fir/input/data1_in.csv"); //Input data file

    input = complexRead16Unscaled(data);
    BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << data);
    int i = input.size(); //Number of inputs

    string taps("./data/fir/input/taps1.txt");
    vector<FixedComplex16> tap;
    tap = complexRead16Unscaled(taps);//Reads in taps from file
    BOOST_REQUIRE_MESSAGE(!tap.empty(), "Could not read from " << taps);
    int j = tap.size();

    string answersFile("./data/fir/answers/answers1.csv"); //Answers data file
    answers = complexRead16Unscaled(answersFile);
    BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << answersFile);
    int l = answers.size();

    FixedFIR fir(j, tap); //Creates instance of fixed FIR filter given j taps.
    for (int k = 0; k < i; k++)
    {
        filter_io_t data;
        data = input[k];
        fir.input(data); //Filters data
        filter_io_t output_sample;
        fir.output(output_sample);
        output.push_back(output_sample.fcn);
    }
    for (int k = 0; k < l; k++) {
        BOOST_CHECK_MESSAGE(
                abs(output[k].real() - answers[k].real())
                        < .001,
                output[k].real() << " is not the same as " << answers[k].real());

    } //Compares all outputs with solution to ensure they are .001 within each other.
}

CSIM_TEST_CASE(COMPLEX_FILTER)
{

//    FixedComplex16 input[1024]; //Array to hold inputs
//    FixedComplex16 output[1024]; //Array to hold outputs
//    double realAnswers[1024];
//    double imagAnswers[1024];
   vector<FixedComplex16> tap(100);
//
//    string data("./data/fir/input/data2_in.csv"); //Input data file
//
//    ifstream in(data.c_str());
//    BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not read from " << data);
    char ** ptr;

    typedef tokenizer<escaped_list_separator<char> > Tokenizer;

    vector<string> vec;
    string line;
//    int i = 0;
//    while (getline(in, line)) {
//        Tokenizer tok(line);
//        vec.assign(tok.begin(), tok.end());
//        double real = atof(vec[0].c_str());
//        double imag = atof(vec[1].c_str());
//        input[i].real(real);
//        input[i].imag(imag);
//        //std::cout << "input = " << input[i] << std::endl;
//        //std::cout << "raw = (" << real << ", " << imag << ")" << std::endl;
//        i++;
//    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.
    string taps("./data/fir/input/taps2.txt");
//
    ifstream in2(taps.c_str());
    BOOST_REQUIRE_MESSAGE(in2.is_open(), "Could not read from " << taps);
    typedef tokenizer<escaped_list_separator<char> > Tokenizer;

    int j = 0; //number of taps

    while (getline(in2, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        tap[j].real(atof(vec[0].c_str()));
        j++;
    } //Reads in taps
//
//    string data3("./data/fir/answers/answers2.csv"); //Answers data file
//
//    ifstream in3(data3.c_str());
//    BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not read from " << data3);
//    int l = 0;//Number of answers
//    while (getline(in3, line)) {
//        Tokenizer tok(line);
//        vec.assign(tok.begin(), tok.end());
//        realAnswers[l] = atof(vec[0].c_str());
//        imagAnswers[l] = atof(vec[1].c_str());
//        l++;
//    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.
//
//    FixedFIR fir(j, tap); //Creates instance of fixed FIR filter given j taps.
//    for (int k = 0; k < i; k++) {
//        filter_io_t data;
//        data = input[k];
//        fir.input(data); //Filters data
//        filter_io_t output_sample;
//        fir.output(output_sample);
//        output[k] = output_sample.fcn;
//     }
//
//    for (int k = 0; k < 40; k++) {
//        BOOST_CHECK_MESSAGE(
//                abs(output[k].real() - realAnswers[k]) < .001,
//                input[k].real() << " is not the same as " << realAnswers[k]);
//        BOOST_CHECK_MESSAGE(
//                abs(output[k].imag() - imagAnswers[k]) < .001,
//                output[k].imag() << " is not the same as " << imagAnswers[k]);
//
//
//    }
    vector<FixedComplex16> input; //Vector to hold inputs
    vector<FixedComplex16> output; //Vector to hold outputs
    vector<FixedComplex16> answers;
//    vector<FixedComplex16> tap;

    string data("./data/fir/input/data2_in.csv"); //Input data file
    input = complexRead16Unscaled(data);
    BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << data);
    int i = input.size();

//
//    string taps("./data/fir/input/taps2.txt");
//    tap = complexRead16Unscaled(taps);
//    BOOST_REQUIRE_MESSAGE(!tap.empty(), "Could not read from " << taps);
//    typedef tokenizer<escaped_list_separator<char> > Tokenizer;
//    int j = taps.size(); //number of taps
//    cout << j << endl;
//    for (int t = 0; t < j; t++) {
//    	cout << tap[t].real().range().to_int64()/32768.0 << endl;
//    	cout << tap[t].imag().range().to_int64()/32768.0 << endl;
//    }

    string data3("./data/fir/answers/answers2.csv"); //Answers data file
    answers = complexRead16Unscaled(data3);
    BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << data3);
    int l = answers.size();//Number of answers

    FixedFIR fir(j, tap); //Creates instance of fixed FIR filter given j taps.
    for (int k = 0; k < i; k++) {
        filter_io_t data;
        data = input[k];
        fir.input(data); //Filters data
        filter_io_t output_sample;
        fir.output(output_sample);
        output.push_back(output_sample.fcn);
     }

    for (int k = 0; k < 40; k++) {
        BOOST_CHECK_MESSAGE(
                abs(output[k].real() - answers[k].real()) < .001,
                input[k].real() << " is not the same as " << answers[k]);
        BOOST_CHECK_MESSAGE(
                abs(output[k].imag() - answers[k].imag()) < .001,
                output[k].imag() << " is not the same as " << answers[k]);


    } //Compares all outputs with solution to ensure they are .001 within each other.

}

CSIM_TEST_SUITE_END()

