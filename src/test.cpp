#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE Hello
#include <boost/test/included/unit_test.hpp>
#include "systemc.h"
 
int add(int i, int j)
{
    return i + j;
}
 
BOOST_AUTO_TEST_CASE(universeInOrder)
{
    BOOST_CHECK(add(2, 2) == 4);

}


BOOST_AUTO_TEST_CASE(systemCMultiplyWrap)
{
	sc_uint<4> count = 15;
	sc_uint<4> mul = 14;

	sc_uint<4> res;

	res = count * mul;

	BOOST_CHECK_EQUAL(res.to_int(), 2);
}


BOOST_AUTO_TEST_CASE(systemCMultiplyMax)
{
	sc_uint<4> count = 15;
	sc_uint<4> mul = 15;

	sc_uint<8> res;

	res = count * mul;

	BOOST_CHECK_EQUAL(res.to_int(), 225);
}
