#include <test/unit_test.hpp>
#include <types/fixedcomplex.hpp>

#include <cfloat>
#include <bitset>
#include <string>

using namespace std;
CSIM_TEST_SUITE_BEGIN(Fixed_Logger)
//
//CSIM_TEST_CASE(Number_Of_Operations)
//{
//	FixedPoint <32, 1>  v(-(1/3.0));
//	FixedPoint <13, 2>  w(.5);
//	for (int i = 0; i < 1; i++) {
//		v = (v+w);
//	}
//
//
//	for (int i = 0; i < 3; i++) {
//		v - v;
//	}
//
//	for (int i = 0; i < 3; i++) {
//		v * v;
//	}
//
//	for (int i = 0; i < 7; i++) {
//		v / v;
//	}
//
//	#ifdef FIXED_POINT_PROFILER_ENABLE
////		sc_dt::scfx_rep::printLog();
//		BOOST_CHECK(sc_dt::scfx_rep::additions == 1);
//		BOOST_CHECK(sc_dt::scfx_rep::subtractions == 3);
//		BOOST_CHECK(sc_dt::scfx_rep::multiplications == 3);
//		BOOST_CHECK(sc_dt::scfx_rep::divisions == 7);
//		sc_dt::scfx_rep::clear(); //Resets all values to 0
////		sc_dt::scfx_rep::printLog();
//		BOOST_CHECK(sc_dt::scfx_rep::additions == 0);
//		BOOST_CHECK(sc_dt::scfx_rep::subtractions == 0);
//		BOOST_CHECK(sc_dt::scfx_rep::multiplications == 0);
//		BOOST_CHECK(sc_dt::scfx_rep::divisions == 0);
//
//		sc_dt::scfx_rep::clear();
//	#endif
//
//
//	FixedComplex32 fc32(1);
////	for (int i = 0; i < 1; i++) {
////		fc32 = fc32 + fc32;
////		}
//
////		for (int i = 0; i < 3; i++) {
////			fc32 - fc32;
////		}
//
////		for (int i = 0; i < 3; i++) {
////			fc32 * fc32;
////		}
////
////		for (int i = 0; i < 7; i++) {
////			fc32 / fc32;
////		}
//
////	#ifdef FIXED_POINT_PROFILER_ENABLE
////		sc_dt::scfx_rep::printLog();
////		BOOST_CHECK(sc_dt::scfx_rep::additions == 2);
//////		BOOST_CHECK(sc_dt::scfx_rep::subtractions == 6);
//////		BOOST_CHECK(sc_dt::scfx_rep::multiplications == 6);
////		BOOST_CHECK(sc_dt::scfx_rep::divisions == 14);
////		sc_dt::scfx_rep::clear();
////		sc_dt::scfx_rep::printLog();
////		BOOST_CHECK(sc_dt::scfx_rep::additions == 0);
////		BOOST_CHECK(sc_dt::scfx_rep::subtractions == 0);
////		BOOST_CHECK(sc_dt::scfx_rep::multiplications == 0);
////		BOOST_CHECK(sc_dt::scfx_rep::divisions == 0);
////
////
////	#endif
//
//
//}
////
//CSIM_TEST_CASE(Overflow_FixedPoint)
//{
//	FixedPoint <4, 4>  v(1);
//
//	for (int i = 0; i < 10; i++) {
//		v = v + v;// 2 4 -8 0 0
//	}
//
//	#ifdef FIXED_POINT_PROFILER_ENABLE
////		sc_dt::scfx_rep::printLog();
//		BOOST_CHECK(sc_dt::scfx_rep::overflows == 2);// 4 + 4 = -8; -8 + -8 = 0
//		sc_dt::scfx_rep::clear();
//	#endif
//
//	FixedPoint <4,4> w(2);
//	for (int i = 0; i < 4; i++) {
//		w = w * w;
//	}
//
//	#ifdef FIXED_POINT_PROFILER_ENABLE
////		sc_dt::scfx_rep::printLog();
//		BOOST_CHECK(sc_dt::scfx_rep::overflows == 1);// 4*4 = 0
//		sc_dt::scfx_rep::clear();
//	#endif
//
//	FixedPoint <16, 1> x(.99);
//
//	for (int i = 0; i < 4; i++) {
//		x = x * x;
//	}
//
//	#ifdef FIXED_POINT_PROFILER_ENABLE
////		sc_dt::scfx_rep::printLog();
//		BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);
//	#endif
//
////		cout << "Value: " << x.range().to_int64() << endl;
////			cout << "Flag: " << x.overflow_flag() << endl << endl;
//
//}
//
//CSIM_TEST_CASE(Overflow_FixedComplex16)
//{
//	#ifdef FIXED_POINT_PROFILER_ENABLE
//		sc_dt::scfx_rep::clear();
//	#endif
//	FixedComplex16 fc16(.99,0);
//
//	for (int i = 0; i < 10; i++) {
//		fc16 = fc16 + fc16;
//	}
//
//	#ifdef FIXED_POINT_PROFILER_ENABLE
////		sc_dt::scfx_rep::printLog();
//		BOOST_CHECK(sc_dt::scfx_rep::overflows == 5);
//		sc_dt::scfx_rep::clear();
//		BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);
//	#endif
//
//	fc16.real(.99);
//	for (int i = 0; i < 10; i++) {
//		fc16 = fc16 * fc16;
//	}
//
//	#ifdef FIXED_POINT_PROFILER_ENABLE
////		sc_dt::scfx_rep::printLog();
//		BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);
//		sc_dt::scfx_rep::clear();
//		BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);
//	#endif
//
//	fc16.real(.5);
//	fc16.imag(.5);
//
//
//	for (int i = 0; i < 10; i++) {
//		fc16 = fc16 - FixedComplex16(.5,.5);
//	}
//
//
//	#ifdef FIXED_POINT_PROFILER_ENABLE
////		sc_dt::scfx_rep::printLog();
//		BOOST_CHECK(sc_dt::scfx_rep::overflows == 4);
//		sc_dt::scfx_rep::clear();
//		BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);
//	#endif
//}
//
//CSIM_TEST_CASE(Precision_Loss_FixedPoint)
//{
//	#ifdef FIXED_POINT_PROFILER_ENABLE
//		sc_dt::scfx_rep::clear();
//	#endif
//	FixedPoint <5, 5>  v(5);
//
//	for (int i = 0; i < 10; i++) {
//		  v = (v / 2);// 2 1 0 0 0...
//	}
//
//	#ifdef FIXED_POINT_PROFILER_ENABLE
//		BOOST_CHECK(sc_dt::scfx_rep::quantizations == 2);// 1 * 1 = 0
//		sc_dt::scfx_rep::clear();
//	#endif
////		cout << "Value: " << v.range().to_int64() << endl;
////		cout << sc_dt::scfx_rep::quantizations << endl;
//}
//
//CSIM_TEST_CASE(MinMax_Zeroes_FixedPoint)
//{
//	#ifdef FIXED_POINT_PROFILER_ENABLE
//		sc_dt::scfx_rep::clear();
//	#endif
//	FixedPoint<5,5> v(4);
//	FixedPoint<5,5> w(4);
//
//	FixedPoint<10,8> z = v+w;
//	cout << (v+w).to_bin() << endl;;
//
//#ifdef FIXED_POINT_PROFILER_ENABLE
//	sc_dt::scfx_rep::printZeroes();
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][5].getMax() == 1);
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][5].getMin() == 0);
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[10][8].getMax() == 3);
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[10][8].getMin() == 3);
//	sc_dt::scfx_rep::clear();
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][5].getMax() == 0);
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][5].getMin() == 1024);
//#endif
//	FixedPoint<5,1> v2(.5);
//	FixedPoint<5,1> w2(.5);
//
//	FixedPoint<5,2> z2 = v2 + w2;
//#ifdef FIXED_POINT_PROFILER_ENABLE
//	sc_dt::scfx_rep::printZeroes();
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][1].getMax() == 0);
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][1].getMin() == -1);
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][2].getMax() == 0);
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][2].getMin() == 0);
//	sc_dt::scfx_rep::clear();
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][1].getMax() == 0);
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[5][1].getMin() == 1024);
//#endif
//
//	FixedPoint <32, 16>  y = v + w;
//}
//
//CSIM_TEST_CASE(MinMax_Zeroes_FixedComplex)
//{
//#ifdef FIXED_POINT_PROFILER_ENABLE
//	sc_dt::scfx_rep::clear();
//#endif
//	FixedComplex16 v(.125,.25);
//	FixedComplex16 w(.75,.5);
//	FixedComplex16 z = v + w;
//
//#ifdef FIXED_POINT_PROFILER_ENABLE
//	sc_dt::scfx_rep::printZeroes();
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[16][1].getMax() == 2);
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[16][1].getMin() == 0);
//	sc_dt::scfx_rep::clear();
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[16][1].getMax() == 0);
//	BOOST_CHECK(sc_dt::scfx_rep::zeroes->vals[16][1].getMin() == 1024);
//#endif
//
//}

CSIM_TEST_CASE(Size_Warning)
{
		FixedPoint<5,3> v2(.5);
		FixedPoint<5,2> w2(.5);

		FixedPoint<10,1> y = w2 + v2;
}


CSIM_TEST_SUITE_END()
