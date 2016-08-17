#include <test/unit_test.hpp>
#include <types/fixedcomplex.hpp>

#include <cfloat>



CSIM_TEST_SUITE_BEGIN(Fixed_Logger)

CSIM_TEST_CASE(Number_Of_Operations)
{
	FixedPoint <4, 4>  v(1);
	cout << "Value: " << v.range().to_int64() << endl;
	cout << "Flag: " << v.overflow_flag() << endl << endl;
	for (int i = 0; i < 8; i++)
	{
		v = v + v;
		cout << "Value: " << v.range().to_int64() << endl;
			cout << "Flag: " << v.overflow_flag() << endl << endl;
	}

	for (int i = 0; i < 3; i++)
	{
		v - v;
	}

	for (int i = 0; i < 3; i++)
	{
		v * v;
	}

	for (int i = 0; i < 7; i++)
	{
		v / v;
	}

	#ifdef FIXED_POINT_PROFILER_ENABLE
		sc_dt::scfx_rep::printLog();
		BOOST_CHECK(sc_dt::scfx_rep::additions == 20);
		BOOST_CHECK(sc_dt::scfx_rep::subtractions == 3);
		BOOST_CHECK(sc_dt::scfx_rep::multiplications == 3);
		BOOST_CHECK(sc_dt::scfx_rep::divisions == 7);
		sc_dt::scfx_rep::clearOperations();
		sc_dt::scfx_rep::printLog();
		BOOST_CHECK(sc_dt::scfx_rep::additions == 0);
		BOOST_CHECK(sc_dt::scfx_rep::subtractions == 0);
		BOOST_CHECK(sc_dt::scfx_rep::multiplications == 0);
		BOOST_CHECK(sc_dt::scfx_rep::divisions == 0);


	#endif


}

CSIM_TEST_SUITE_END()
