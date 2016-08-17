#include <test/unit_test.hpp>
#include <types/fixedcomplex.hpp>

#include <cfloat>



CSIM_TEST_SUITE_BEGIN(Fixed_Logger)

CSIM_TEST_CASE(Number_Of_Operations)
{
	FixedPoint <4, 1>  v(0);
	for (int i = 0; i < 1; i++)
	{
		v + v;
	}
	v.overflow_flag();
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
		sc_dt::scfx_rep::printOperations();
		sc_dt::scfx_rep::clearOperations();
		sc_dt::scfx_rep::printOperations();
	#endif


}

CSIM_TEST_SUITE_END()
