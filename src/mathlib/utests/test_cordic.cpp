#include <test/unit_test.hpp>

#include <mathlib/cordic.hpp>
#include <iostream>

using namespace std;
CSIM_TEST_SUITE_BEGIN(CordicSuite)

CSIM_TEST_CASE(CordicFunctionality)
{
    //string outFile = "data/cordic/output/sinwave.txt";
    //ofstream out(outFile.c_str());

    //string outFile2 = "data/cordic/output/clockup.txt";
    //ofstream out2(outFile2.c_str());

    //string outFile3 = "data/cordic/output/clockdown.txt";
    //ofstream out3(outFile3.c_str());

    cordic c;
    cordic_complex_t a(1.0, 0.0);
    cordic_complex_t b(0.0, 1.0);
    cordic_scalar_t cosdown;
    cordic_scalar_t cosup;
    cordic_scalar_t sinup;
    cordic_scalar_t sindown;
    for (double i = 0; i < 4 * M_PI; i = i + .01) {
        cordic_theta_t theta = i;
        c.calculate(theta, a, b, &sinup, &sindown, &cosup, &cosdown);
        	//cout << "Cosine: " << cosdown << endl;
        	//cout << "Sine: " << sinup << endl;
        //	out << cosdown/32768.0<<",";
//		out << cosup/32768.0 << endl;

        //out2 << cosup / 32768.0 << ",";
        //out2 << sinup / 32768.0 << endl;
        //out3 << cosdown / 32768.0 << ",";
        //out3 << sindown / 32768.0 << endl;
        //sinup = c.sin(j);
        //out << sinup / 32768.0 << endl;
    }

    //out.close();

}
CSIM_TEST_SUITE_END()
