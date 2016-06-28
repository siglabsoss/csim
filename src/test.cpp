#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE Complex
#include <boost/test/included/unit_test.hpp>
#include "fixedcomplex.h"
#include <iostream>


BOOST_AUTO_TEST_CASE(Equal_and_notequal)
{

	sc_int<16> a = (4);
	sc_int<16> b = (12);

	FixedComplex<16> c1(a,b);
	FixedComplex<16> c2(b,a);

	BOOST_CHECK( c1 == c1 );//Check == when true
	BOOST_CHECK( !(c1 != c1) );//Check != when true
	BOOST_CHECK( !(c1==c2) ); //Check == when false
	BOOST_CHECK( c1 != c2 ); //Check != when false



}

BOOST_AUTO_TEST_CASE(CONSTRUCTORS)
{

	sc_int<16> a = (65536);
	sc_int<16> b = (12);

	FixedComplex<16> c1(a,b);
	FixedComplex<16> c2(b,a);
	sc_int<16> z = 0;
	BOOST_CHECK( c1.real == z );//Tests rollover and constructor
	BOOST_CHECK( c1.real == a );//Tests constructor
	BOOST_CHECK(c1.imag == b); //Tests constructor
	BOOST_CHECK(c1.real == c2.imag); //Tests constructor
	BOOST_CHECK(c1.imag==c2.real); //Tests constructor

	FixedComplex<16> c3;
	BOOST_CHECK( c3.real.to_int() == 0 );//Tests default
	BOOST_CHECK(c3.imag.to_int() == 0); //Tests default

	FixedComplex<16> c4(1);
	BOOST_CHECK( c4.real.to_int() == 1 );//Tests constructor with 1 arg
	BOOST_CHECK(c4.imag.to_int() == 0); //Tests constructor with 1 arg



}

BOOST_AUTO_TEST_CASE(ADDING)
{
	sc_int<16> a = (2);
	sc_int<16> b = (1);

	FixedComplex<16> c1(a,b);
	FixedComplex<16> c2(b,a);

	FixedComplex<16> c3;
	c3 = c1 + c2;
	BOOST_CHECK(c3.real.to_int() == (3) ); //Tests basic add
	BOOST_CHECK(c3.imag.to_int() == (3) ); //Tests basic add

	c1.real = -5;
	c1.imag = -10;


	c3 = c1 + c2;
	BOOST_CHECK(c3.real.to_int() == (-4));//Tests adding negatives and positive
	BOOST_CHECK(c3.imag.to_int() == (-8));//Tests adding negatives and positives

	c2.real = -2;
	c2.imag = -5;

	c3 = c1 + c2;
	BOOST_CHECK(c3.real.to_int() == (-7));//Tests adding negatives
	BOOST_CHECK(c3.imag.to_int() == (-15));//Tests adding negatives

	c1.real = 1;
	c1.imag = 1;
	c2.real = 32767;
	c2.imag = 32767;

	c3 = c1 + c2;
	BOOST_CHECK(c3.real.to_int() == (-32768));//Tests rollover
	BOOST_CHECK(c3.imag.to_int() == (-32768));//Tests rollover
}

BOOST_AUTO_TEST_CASE(SUBTRACTING)
{
	sc_int<16> a = (2);
	sc_int<16> b = (4);
	sc_int<16> c = (1);
	sc_int<16> d = (2);

	FixedComplex<16> c1(a,b);
	FixedComplex<16> c2(c,d);

	FixedComplex<16> c3;
	c3 = c1 - c2;
	BOOST_CHECK(c3.real.to_int() == 1 );//Tests basic subtraction
	BOOST_CHECK(c3.imag.to_int() == 2 );//Tests basic subtraction

	c1.real = -5;
	c1.imag = -10;
	c2.real = -3;
	c2.imag = -1;


	c3 = c1 - c2;
	BOOST_CHECK(c3.real.to_int() == (-2));//Tests subtracting negatives
	BOOST_CHECK(c3.imag.to_int() == (-9));//Tests subtracting negatives


	c1.real = -5;
	c1.imag = -10;
	c2.real = 1;
	c2.imag = 1;


	c3 = c1 - c2;
	BOOST_CHECK(c3.real.to_int() == (-6));//Tests subtracting negatives and positives
	BOOST_CHECK(c3.imag.to_int() == (-11) );//Tests subtracting negatives and positives

}

BOOST_AUTO_TEST_CASE(MULTIPLYING)
{
	sc_int<16> a = (2);
	sc_int<16> b = (1);

	FixedComplex<16> c1(a,b);
	FixedComplex<16> c2(b,a);

	FixedComplex<16> c3;

	c3 = c1 * c2;
	BOOST_CHECK(c3.real.to_int() == 0);//Normal multiply
	BOOST_CHECK(c3.imag.to_int() == 5);//Normal multiply

	sc_int<16> c = (4);
	sc_int<16> d = (6);

	FixedComplex<16> c4(a,b);
	FixedComplex<16> c5(c,d);

	FixedComplex<16> c6;

	c6 = c4 * c5;

	BOOST_CHECK(c6.real.to_int() == 2);//Negative multiply
	BOOST_CHECK(c6.imag.to_int() == 16);//Negative multiply

}


BOOST_AUTO_TEST_CASE(DIVIDING)
{

	sc_int<16> a = (2);
	sc_int<16> b = (2);
	sc_int<16> c = (0);
	sc_int<16> d = (1);

	FixedComplex<16> c1(a,b);
	FixedComplex<16> c2(c,d);

	FixedComplex<16> c3;

	c3 = c1/c2;
	BOOST_CHECK(c3.real.to_int() == 2);//Tests normal division
	BOOST_CHECK(c3.imag.to_int() == -2);//Tests normal division


	c1.real = 4;
	c1.imag = 5;
	c2.real = 2;
	c2.imag = 6;

	c3 = c1/c2;
	BOOST_CHECK(c3.real.to_int() == 0);//tests truncating
	BOOST_CHECK(c3.imag.to_int() == 0);//Tests truncating


	c1.real = 110;
	c1.imag = 50;
	c2.real = 4;
	c2.imag = 6;

	c3 = c1/c2;

	BOOST_CHECK(c3.real.to_int() == 14);//tests truncating
	BOOST_CHECK(c3.imag.to_int() == -8);//Tests truncating

	c1.real = 20;
	c1.imag = -100;
	c2.real = 4;
	c2.imag = 6;

	c3 = c1/c2;
	BOOST_CHECK(c3.real.to_int() == -10);//Tests negative nums
	BOOST_CHECK(c3.imag.to_int() == -10);//Tests negative nums
}




BOOST_AUTO_TEST_CASE(SHIFTING)
{

	sc_int<16> a = (2);
	sc_int<16> b = (1);
	FixedComplex<16> c1(a,b);

	c1 = c1<<3;
	BOOST_CHECK(c1.real.to_int() == 16);//Normal left shift
	BOOST_CHECK(c1.imag.to_int() == 8);//Normal left shift

	c1.real = 32768;
	c1.imag = 0;

	c1 = c1<<3;
	BOOST_CHECK(c1.real.to_int() == 0);//Left shift out all 1's
	BOOST_CHECK(c1.imag.to_int() == 0);//Left shift out all 1's


	c1.real = 4;
	c1.imag = 1;
	c1 = c1>>1;
	BOOST_CHECK(c1.real.to_int() == 2);//Normal right shift
	BOOST_CHECK(c1.imag.to_int() == 0);//Normal right shift

	c1 = c1>>5;
	BOOST_CHECK(c1.real.to_int() == 0);//Right shift out all 1's
	BOOST_CHECK(c1.imag.to_int() == 0);//Right shift out all 1's




}



BOOST_AUTO_TEST_CASE(CASTING)
{
	sc_int<16> a = (2);
	sc_int<16> b = (1);
	FixedComplex<16> c1(a,b);
	FixedComplex<32> c2;
	c2 = c1.to_32();
	BOOST_CHECK(c2.real.to_int() == 2);//Testing conversion up with no change
	BOOST_CHECK(c2.imag.to_int() == 1);//Testing conversion up with no change
	
	c2.real = 4;
	c2.imag = 3;
	c1 = c2.to_16();
	BOOST_CHECK(c1.real.to_int() == 4);//Testing conversion down with no change
	BOOST_CHECK(c1.imag.to_int() == 3);//Testing conversion down with no change
	
	c2.real = 536936449; //2e29 + 1
	c2.imag = 65538; //2e16 + 2
	c1 = c2.to_16();
	BOOST_CHECK(c1.real.to_int() == 1);//Testing conversion down with truncation
	BOOST_CHECK(c1.imag.to_int() == 2);//Testing conversion down with truncation
	
	
	
}
