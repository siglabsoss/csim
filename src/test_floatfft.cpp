
// In all sub-test files don't use /included/ version
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <iostream>

using namespace std;

#include "floatfft.h"
#include "utils.h"

BOOST_AUTO_TEST_SUITE(FloatFFT)

BOOST_AUTO_TEST_CASE(FIR_TEST)
{

	const int fft_size = 8;
	int index;

	floatfft fft(fft_size);
	floatfftbuffer saved(fft_size);
	fft.stages[fft.stagecount-1].next = &saved;

	int i;

	for(i = 0; i < 10; i++)
	{
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

	float r1,r2,i1,i2;

	for(k = 0; k < fft_size*2; k++)
	{
		index = reverseBits(fft_size, k%fft_size);

		r1 = expected[index].real();
		i1 = expected[index].imag();

		r2 = saved.buf[k].real();
		i2 = saved.buf[k].imag();

		BOOST_CHECK_CLOSE(r1, r2, tolerance);
		BOOST_CHECK_CLOSE(i1, i2, tolerance);
	}


}


BOOST_AUTO_TEST_SUITE_END()
