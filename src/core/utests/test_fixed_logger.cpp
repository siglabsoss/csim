#include <test/unit_test.hpp>
#include <types/fixedcomplex.hpp>

#include <cfloat>
#include <bitset>
#include <string>

using namespace std;
CSIM_TEST_SUITE_BEGIN(Fixed_Logger)

#ifdef FIXED_POINT_PROFILER_ENABLE

CSIM_TEST_CASE(Number_Of_Operations)
{
	sc_dt::scfx_rep::warningLevel = 0;
	FixedPoint <4, 4>  v(-(1/3.0));
	FixedPoint <4, 4>  w(.5);

	for (int i = 0; i < 1; i++) {
		(v+w);
	}

	for (int i = 0; i < 3; i++) {
		v - v;
	}

	for (int i = 0; i < 3; i++) {
		v * v;
	}

	for (int i = 0; i < 7; i++) {
		v / v;
	}

//		sc_dt::scfx_rep::printLog();
	BOOST_CHECK(sc_dt::scfx_rep::additions == 1);
	BOOST_CHECK(sc_dt::scfx_rep::subtractions == 3);
	BOOST_CHECK(sc_dt::scfx_rep::multiplications == 3);
	BOOST_CHECK(sc_dt::scfx_rep::divisions == 7);
	sc_dt::scfx_rep::clear(); //Resets all values to 0
//		sc_dt::scfx_rep::printLog();
	BOOST_CHECK(sc_dt::scfx_rep::additions == 0);
	BOOST_CHECK(sc_dt::scfx_rep::subtractions == 0);
	BOOST_CHECK(sc_dt::scfx_rep::multiplications == 0);
	BOOST_CHECK(sc_dt::scfx_rep::divisions == 0);

	sc_dt::scfx_rep::clear();

}

CSIM_TEST_CASE(Overflow_FixedPoint)
{
	FixedPoint <4, 4>  v(1);

	for (int i = 0; i < 10; i++) {
		v = v + v;// 2 4 -8 0 0
	}

//		sc_dt::scfx_rep::printLog();
		BOOST_CHECK(sc_dt::scfx_rep::overflows == 2);// 4 + 4 = -8; -8 + -8 = 0
		sc_dt::scfx_rep::clear();

	FixedPoint <4,4> w(2);
	for (int i = 0; i < 4; i++) {
		w = w * w;
	}

//		sc_dt::scfx_rep::printLog();
		BOOST_CHECK(sc_dt::scfx_rep::overflows == 1);// 4*4 = 0
		sc_dt::scfx_rep::clear();

	FixedPoint <16, 1> x(.99);

	for (int i = 0; i < 4; i++) {
		x = x * x;
	}

//		sc_dt::scfx_rep::printLog();
	BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);
//		cout << "Value: " << x.range().to_int64() << endl;
//			cout << "Flag: " << x.overflow_flag() << endl << endl;

}

CSIM_TEST_CASE(Overflow_FixedComplex16)
{
	sc_dt::scfx_rep::clear();
	FixedComplex16 fc16(.99,0);

	for (int i = 0; i < 10; i++) {
		fc16 = fc16 + fc16;
	}

	BOOST_CHECK(sc_dt::scfx_rep::overflows == 5);
	sc_dt::scfx_rep::clear();
	BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);

	fc16.real(.99);
	for (int i = 0; i < 10; i++) {
		fc16 = fc16 * fc16;
	}

//		sc_dt::scfx_rep::printLog();
	BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);
	sc_dt::scfx_rep::clear();
	BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);

	fc16.real(.5);
	fc16.imag(.5);


	for (int i = 0; i < 10; i++) {
		fc16 = fc16 - FixedComplex16(.5,.5);
	}


//		sc_dt::scfx_rep::printLog();
	BOOST_CHECK(sc_dt::scfx_rep::overflows == 4);
	sc_dt::scfx_rep::clear();
	BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);
}

CSIM_TEST_CASE(Precision_Loss_FixedPoint)
{
	sc_dt::scfx_rep::clear();
	FixedPoint <5, 5>  v(5);

	for (int i = 0; i < 10; i++) {
		  v = (v / 2);// 2 1 0 0 0...
	}

	BOOST_CHECK(sc_dt::scfx_rep::quantizations == 2);// 1 * 1 = 0
	sc_dt::scfx_rep::clear();
//		cout << "Value: " << v.range().to_int64() << endl;
//		cout << sc_dt::scfx_rep::quantizations << endl;
}

CSIM_TEST_CASE(MinMax_Zeroes_FixedPoint)
{
	sc_dt::scfx_rep::clear();
	FixedPoint<5,5> v(4);
	FixedPoint<5,5> w(4);

	FixedPoint<10,8> z = v+w;
	cout << (v+w).to_bin() << endl;;
//	sc_dt::scfx_rep::printZeroes();
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][5].getMax() == 1);
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][5].getMin() == 0);
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[10][8].getMax() == 3);
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[10][8].getMin() == 3);
	sc_dt::scfx_rep::clear();
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][5].getMax() == 0);
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][5].getMin() == 1024);
	FixedPoint<5,1> v2(.5);
	FixedPoint<5,1> w2(.5);

	FixedPoint<5,2> z2 = v2 + w2;

//	sc_dt::scfx_rep::printZeroes();
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][1].getMax() == 0);
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][1].getMin() == -1);
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][2].getMax() == 0);
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][2].getMin() == 0);
	sc_dt::scfx_rep::clear();
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][1].getMax() == 0);
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][1].getMin() == 1024);

}

CSIM_TEST_CASE(MinMax_Zeroes_FixedComplex)
{

	sc_dt::scfx_rep::clear();

	FixedComplex16 v(.125,.25);
	FixedComplex16 w(.75,.5);
	FixedComplex16 z = v + w;

	//sc_dt::scfx_rep::printZeroes();
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[16][1].getMax() == 2);
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[16][1].getMin() == 0);
	sc_dt::scfx_rep::clear();
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[16][1].getMax() == 0);
	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[16][1].getMin() == 1024);


}

CSIM_TEST_CASE(Addition_Warning)
{

	sc_dt::scfx_rep::warningLevel = 0;//0 - disable, 1 = warning, 2 = exit on warning
	FixedPoint <4, 4>  v(0);
	FixedPoint <4, 4>  w(0);
	FixedPoint <5, 5> z(0);
	z = (v + w); //No warning

	FixedPoint <6, 3>  v2(0);
	FixedPoint <4, 4>  w2(0);
	z = (v2 + w2);//Precision warning

	FixedPoint <5, 5>  v3(0);
	FixedPoint <4, 4>  w3(0);
	z = (v3 + w3);//Range warning

	FixedPoint <6, 1>  v4(0);
	FixedPoint <6, 1>  w4(0);
	z = (v3 + w3);//Precision and range warning


}

CSIM_TEST_CASE(Subtraction_Warning)
{

	sc_dt::scfx_rep::warningLevel = 0;//0 - disable, 1 = warning, 2 = exit on warning
	FixedPoint <4, 4>  v(0);
	FixedPoint <4, 4>  w(0);
	FixedPoint <5, 5> z;
	z = (v - w); //No warning

	FixedPoint <6, 3>  v2(0);
	FixedPoint <4, 4>  w2(0);
	z = (v2 - w2);//Precision warning

	FixedPoint <5, 5>  v3(0);
	FixedPoint <4, 4>  w3(0);
	z = (v3 - w3);//Range warning

	FixedPoint <6, 1>  v4(0);
	FixedPoint <6, 1>  w4(0);
	z = (v4 - w4);//Precision and range warning

}

CSIM_TEST_CASE(Multiplication_Warning)
{

	sc_dt::scfx_rep::warningLevel = 0;//0 - disable, 1 = warning, 2 = exit on warning
	FixedPoint <4, 4>  v(0);
	FixedPoint <4, 4>  w(0);
	FixedPoint <8, 8> z;
	z = (v * w); //No warning

	FixedPoint <4, 3>  v2(0);
	FixedPoint <4, 4>  w2(0);
	z = (v2 * w2);//Precision warning

	FixedPoint <5, 5>  v3(0);
	FixedPoint <4, 4>  w3(0);
	z = (v3 * w3);//Range warning

	FixedPoint <5, 5>  v4(0);
	FixedPoint <5, 4>  w4(0);

	z = (v4 * w4);//Precision and range warning

}

CSIM_TEST_CASE(Division_Warning)
{

	sc_dt::scfx_rep::warningLevel = 0;//0 - disable, 1 = warning, 2 = exit on warning
	FixedPoint <4, 4> z;

	FixedPoint <4, 4>  v2(1);
	FixedPoint <4, 4>  w2(1);
	z = (v2 / w2);//Precision warning

	FixedPoint <5, 5>  v4(1);
	FixedPoint <4, 4>  w4(1);
	z = (v4 / w4);//Precision and range warning

}

#endif

CSIM_TEST_SUITE_END()
