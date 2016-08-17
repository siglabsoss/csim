#include <test/unit_test.hpp>
#include <types/fixedcomplex.hpp>

#include <cfloat>



CSIM_TEST_SUITE_BEGIN(Fixed_Logger)

CSIM_TEST_CASE(Number_Of_Operations)
{
	FixedPoint <4, 4>  v(1);

	for (int i = 0; i < 1; i++) {
		v = v + v;
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

	#ifdef FIXED_POINT_PROFILER_ENABLE
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


	#endif

//	sc_dt::scfx_rep::clear();
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
//		for (int i = 0; i < 7; i++) {
//			fc32 / fc32;
//		}
//
//	#ifdef FIXED_POINT_PROFILER_ENABLE
//		sc_dt::scfx_rep::printLog();
//		BOOST_CHECK(sc_dt::scfx_rep::additions == 2);
////		BOOST_CHECK(sc_dt::scfx_rep::subtractions == 6);
////		BOOST_CHECK(sc_dt::scfx_rep::multiplications == 6);
//		BOOST_CHECK(sc_dt::scfx_rep::divisions == 14);
//		sc_dt::scfx_rep::clear();
//		sc_dt::scfx_rep::printLog();
//		BOOST_CHECK(sc_dt::scfx_rep::additions == 0);
//		BOOST_CHECK(sc_dt::scfx_rep::subtractions == 0);
//		BOOST_CHECK(sc_dt::scfx_rep::multiplications == 0);
//		BOOST_CHECK(sc_dt::scfx_rep::divisions == 0);
//
//
//	#endif


}

CSIM_TEST_CASE(Overflow)
{
	FixedPoint <4, 4>  v(1);

	for (int i = 0; i < 10; i++) {
		v = v + v;// 2 4 -8 0 0
	}

	#ifdef FIXED_POINT_PROFILER_ENABLE
//		sc_dt::scfx_rep::printLog();
		BOOST_CHECK(sc_dt::scfx_rep::overflows == 2);// 4 + 4 = -8; -8 + -8 = 0
		sc_dt::scfx_rep::clear();
	#endif

	FixedPoint <4,4> w(2);
	for (int i = 0; i < 4; i++) {
		w = w * w;
	}

	#ifdef FIXED_POINT_PROFILER_ENABLE
//		sc_dt::scfx_rep::printLog();
		BOOST_CHECK(sc_dt::scfx_rep::overflows == 1);// 4*4 = 0
		sc_dt::scfx_rep::clear();
	#endif

	FixedPoint <16, 1> x(.99);

	for (int i = 0; i < 4; i++) {
		x = x * x;
	}

	#ifdef FIXED_POINT_PROFILER_ENABLE
//		sc_dt::scfx_rep::printLog();
		BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);
	#endif

//		cout << "Value: " << x.range().to_int64() << endl;
//			cout << "Flag: " << x.overflow_flag() << endl << endl;
}
CSIM_TEST_SUITE_END()
