#include <test/unit_test.hpp>

#include <iostream>

using namespace std;

#include <filters/floatfft.hpp>
#include <utils/utils.hpp>
#include "systemc.h"

CSIM_TEST_SUITE_BEGIN(FloatFFT)

CSIM_TEST_CASE(FLOAT_FFT_TEST)
{

    const int fft_size = 8;
    int index;

    floatfft fft(fft_size);
    floatfftbuffer saved(fft_size);
    fft.stages[fft.stagecount - 1].next = &saved;

    int i;

    for (i = 0; i < 10; i++) {
        fft.inputandtick(5);
        fft.inputandtick(6);
        fft.inputandtick(8);
        fft.inputandtick(-5);
        fft.inputandtick(6);
        fft.inputandtick(12);
        fft.inputandtick(10);
        fft.inputandtick(9);
    }

    int k;

    // matlab results
    std::vector<complex<float> > expected;
    expected.push_back(complex<float>(51, 0));
    expected.push_back(complex<float>(4.65685424949238, 16.142135623731));
    expected.push_back(complex<float>(-7, -14));
    expected.push_back(complex<float>(-6.65685424949238, 12.142135623731));
    expected.push_back(complex<float>(7, 0));
    expected.push_back(complex<float>(-6.65685424949238, -12.142135623731));
    expected.push_back(complex<float>(-7, 14));
    expected.push_back(complex<float>(4.65685424949238, -16.142135623731));

    const float tolerance = 0.0001;  // measured in percentage

    float r1, r2, i1, i2;

    for (k = 0; k < fft_size * 2; k++) {
        index = reverseBits(fft_size, k % fft_size);

        r1 = expected[index].real();
        i1 = expected[index].imag();

        r2 = saved.buf[k].real();
        i2 = saved.buf[k].imag();

        BOOST_CHECK_CLOSE(r1, r2, tolerance);
        BOOST_CHECK_CLOSE(i1, i2, tolerance);
    }

}

CSIM_TEST_CASE(BLAH)
{

    cout << "hello world" << endl;

//	FixedComplex<8> c1 ( rca)

    float result = 0.2 * 0.2;

    cout << "float " << result << endl;

    const float scalar = 64;

    sc_int<6> f1 = 0.2 * scalar;
    sc_int<6> f2 = 0.2 * scalar;

    cout << "scaled " << f1 << endl;

    sc_int<12> result2 = (sc_int<6> ) f1 * (sc_int<6> ) f2;

//	sc_int<32> result3 = result2 / scalar;

    cout << "fixed<> " << result2 << endl;
    cout << "fixed<> " << (float) result2 / (scalar * scalar) << endl;

}

CSIM_TEST_SUITE_END()
