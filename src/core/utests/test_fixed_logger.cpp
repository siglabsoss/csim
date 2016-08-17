#include <test/unit_test.hpp>
#include <types/fixedcomplex.hpp>

#include <cfloat>



CSIM_TEST_SUITE_BEGIN(Fixed_Logger)

CSIM_TEST_CASE(Number_Of_Operations)
{
	vector<FixedPoint <4, 1> > v(100);
	for (int i = 0; i < 10; i++)
	{
		v[i] + v[i];
	}
//	sc_dt::additions = 5;
	sc_dt::scfx_rep::printOperations();
}

CSIM_TEST_SUITE_END()
