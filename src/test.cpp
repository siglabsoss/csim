#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE Complex
#include <boost/test/included/unit_test.hpp>
#include "fixedcomplex.h"
#include <iostream>
int add(int i, int j)
{
    return i + j;
}
//
//BOOST_AUTO_TEST_CASE(universeInOrder)
//{
//    BOOST_CHECK(add(2, 2) == 4);
//
//}
//
//

//BOOST_AUTO_TEST_CASE(systemCMultiplyWrap)
//{
//	sc_uint<4> count = 15;
//	sc_uint<4> mul = 14;
//
//	sc_uint<4> res;
//
//	res = count * mul;
//
//	BOOST_CHECK_EQUAL(res.to_int(), 2);
//}
//
//
//BOOST_AUTO_TEST_CASE(systemCMultiplyMax)
//{
//	sc_uint<4> count = 15;
//	sc_uint<4> mul = 15;
//
//	sc_uint<8> res;
//
//	res = count * mul;
//
//	BOOST_CHECK_EQUAL(res.to_int(), 125);
//}
//
BOOST_AUTO_TEST_CASE(Equal_and_notequal)
{

	sc_uint<16> a = (4);
	sc_uint<16> b = (12);

	FixedComplex c1(a,b);
	FixedComplex c2(b,a);

	BOOST_CHECK( c1 == c1 );
	BOOST_CHECK( !(c1 != c1) );
	BOOST_CHECK( c1 != c2 );
	BOOST_CHECK( !(c1==c2) );


}

BOOST_AUTO_TEST_CASE(CONSTRUCTORS_AND_ROLLOVER)
{

	sc_uint<16> a = (65536);
	sc_uint<16> b = (12);

	FixedComplex c1(a,b);
	FixedComplex c2(b,a);
	sc_uint<16> z = 0;
	BOOST_CHECK( c1.real == z );
	BOOST_CHECK( c1.real == a );
	BOOST_CHECK(c1.imag == b);
	BOOST_CHECK(c1.real == c2.imag);
	BOOST_CHECK(c1.imag==c2.real);


}

BOOST_AUTO_TEST_CASE(ADDING)
{
	sc_uint<16> a = (2);
	sc_uint<16> b = (1);

	FixedComplex c1(a,b);
	FixedComplex c2(b,a);

	FixedComplex c3;
	c3 = c1 + c2;
	BOOST_CHECK(c3.real == (c1.real + c2.real) );
	BOOST_CHECK(c3.imag == (c1.imag + c2.imag) );

}

BOOST_AUTO_TEST_CASE(SUBTRACTING)
{
	sc_uint<16> a = (2);
	sc_uint<16> b = (1);
	sc_uint<16> c = (0);
	FixedComplex c1(a,b);
	FixedComplex c2(b,a);

	FixedComplex c3;
	c3 = c1 - c2;
	BOOST_CHECK(c3.real.to_int() == 1 );
	BOOST_CHECK(c3.imag.to_int() == 65535 );

}

BOOST_AUTO_TEST_CASE(MULTIPLYING)
{
	sc_uint<16> a = (2);
	sc_uint<16> b = (1);

	FixedComplex c1(a,b);
	FixedComplex c2(b,a);

	FixedComplex c3;

	c3 = c1 * c2;
	BOOST_CHECK(c3.real.to_int() == 0);
	BOOST_CHECK(c3.imag.to_int() == 5);
}


BOOST_AUTO_TEST_CASE(DIVIDING)
{
	sc_uint<16> a = (2);
	sc_uint<16> b = (1);

	FixedComplex c1(a,b);
	FixedComplex c2(b,a);

}
