#include <test/unit_test.hpp>
#include <types/fixedcomplex.hpp>

CSIM_TEST_SUITE_BEGIN(Fixed_Logger)


CSIM_TEST_CASE(Empty)
{}

#ifdef FIXED_POINT_PROFILER_ENABLE

CSIM_TEST_CASE(Number_Of_Operations)
{
	sc_dt::scfx_rep::clear();
	sc_dt::scfx_rep::warningLevel = 0;
	FixedPoint <4, 4>  v(2);
	FixedPoint <4, 4>  w(4);
	FixedPoint <4, 4> z;
	for (int i = 0; i < 1; i++) {
		z = (v+w);
	}

	for (int i = 0; i < 3; i++) {
		z = v - w;
	}

	for (int i = 0; i < 3; i++) {
		z = v * v;
	}

	for (int i = 0; i < 7; i++) {
		z = v / v;
	}

	BOOST_CHECK(sc_dt::scfx_rep::additions == 1);
	BOOST_CHECK(sc_dt::scfx_rep::subtractions == 3);
	BOOST_CHECK(sc_dt::scfx_rep::multiplications == 3);
	BOOST_CHECK(sc_dt::scfx_rep::divisions == 7);
	sc_dt::scfx_rep::clear(); //Resets all values to 0
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

	BOOST_CHECK(sc_dt::scfx_rep::overflows == 2);// 4 + 4 = -8; -8 + -8 = 0
	sc_dt::scfx_rep::clear();

	FixedPoint <4,4> w(2);
	for (int i = 0; i < 4; i++) {
		w = w * w;
	}

		BOOST_CHECK(sc_dt::scfx_rep::overflows == 1);// 4*4 = 0
		sc_dt::scfx_rep::clear();

	FixedPoint <16, 1> x(.99);

	for (int i = 0; i < 4; i++) {
		x = x * x;
	}


	BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);
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

	BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);
	sc_dt::scfx_rep::clear();
	BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);

	fc16.real(.5);
	fc16.imag(.5);


	for (int i = 0; i < 10; i++) {
		fc16 = fc16 - FixedComplex16(.5,.5);
	}

	BOOST_CHECK(sc_dt::scfx_rep::overflows == 4);
	sc_dt::scfx_rep::clear();
	BOOST_CHECK(sc_dt::scfx_rep::overflows == 0);
}

CSIM_TEST_CASE(Precision_Loss_FixedPoint)
{
	sc_dt::scfx_rep::clear();
	FixedPoint <5, 5>  v(5);

	for (int i = 0; i < 10; i++) {
		  v = (v / 2);// 2.5->2, 1, .5->0, 0, 0...
	}

	BOOST_CHECK(sc_dt::scfx_rep::quantizations == 2);// 1 * 1 = 0
	sc_dt::scfx_rep::clear();
}

CSIM_TEST_CASE(MinMax_Zeroes_FixedPoint)
{
	sc_dt::scfx_rep::clear();
	FixedPoint<5,5> v(4);
	FixedPoint<5,5> w(5);
	v+w;

	FixedPoint<10,8> z = v+w;

	std::map<sc_dt::key,sc_dt::minMax>::iterator it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(5,5));//Access minMax zeroes for <5,5>
	BOOST_CHECK(it->second.getMin() == 0);
	BOOST_CHECK(it->second.getMax() == 1);

	it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(10,8)); //Access minMax zeroes for <10,8>
	BOOST_CHECK(it->second.getMin() == 3);
	BOOST_CHECK(it->second.getMax() == 3);

	sc_dt::scfx_rep::clear();
	FixedPoint<5,1> v2(.5);
	FixedPoint<5,1> w2(.5);

	FixedPoint<5,2> z2 = v2 + w2;

	it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(5,1)); //Access minMax zeroes for <5,1>
	BOOST_CHECK(it->second.getMin() == -1);
	BOOST_CHECK(it->second.getMax() == 0);
	it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(5,2)); //Access minMax zeroes for <5,2>
	BOOST_CHECK(it->second.getMin() == 0);
	BOOST_CHECK(it->second.getMax() == 0);
	sc_dt::scfx_rep::clear();

}

CSIM_TEST_CASE(MinMax_Zeroes_Addition_FixedComplex)
{

	sc_dt::scfx_rep::clear();

	FixedComplex16 x(.25,0);
	FixedComplex16 y(.25,0);

	std::map<sc_dt::key,sc_dt::minMax>::iterator it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(16,1)); //Access minMax zeroes for <16,1>
	BOOST_CHECK(it->second.getMin() == 1);// 0 01000000...
	BOOST_CHECK(it->second.getMax() == 15);// 0 01000000...
	x + y;
	it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(16,1)); //Access minMax zeroes for <16,1>
	BOOST_CHECK(it->second.getMin() == 0);// 0 100000...
	BOOST_CHECK(it->second.getMax() == 15);// 0 010000...

	sc_dt::scfx_rep::clear();
	FixedComplex16 z = x + y;

	it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(16,1)); //Access minMax zeroes for <16,1>
	BOOST_CHECK(it->second.getMin() == 0);// 0 100000...
	BOOST_CHECK(it->second.getMax() == 15);// 0 100000...

	sc_dt::scfx_rep::clear();
	FixedComplex16 v(.125,.5);
	FixedComplex16 w(.125,.5);

	it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(16,1)); //Access minMax zeroes for <16,1>
	BOOST_CHECK(it->second.getMin() == 0);// 0 01000000...
	BOOST_CHECK(it->second.getMax() == 2);// 0 01000000...
	w + v;
	it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(16,1)); //Access minMax zeroes for <16,1>
	BOOST_CHECK(it->second.getMin() == -1);// 0 100000...
	BOOST_CHECK(it->second.getMax() == 2);// 0 010000...

	sc_dt::scfx_rep::clear();
	FixedComplex16 u = w + v;

	it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(16,1)); //Access minMax zeroes for <16,1>
	BOOST_CHECK(it->second.getMin() == -1);// 0 100000...
	BOOST_CHECK(it->second.getMax() == 2);// 0 100000...


}

CSIM_TEST_CASE(MinMax_Zeroes_Subtraction_FixedComplex)
{
	std::map<sc_dt::key,sc_dt::minMax>::iterator it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(16,1)); //Access minMax zeroes for <16,1>
	sc_dt::scfx_rep::clear();
	FixedComplex16 a(.125,.5);
	FixedComplex16 b(.125,.5);

	it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(16,1)); //Access minMax zeroes for <16,1>
	BOOST_CHECK(it->second.getMin() == 0);// 0 01000000...
	BOOST_CHECK(it->second.getMax() == 2);// 0 01000000...
	a - b;
	it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(16,1)); //Access minMax zeroes for <16,1>
	BOOST_CHECK(it->second.getMin() == 0);// 0 100000...
	BOOST_CHECK(it->second.getMax() == 15);// 0 010000...

	sc_dt::scfx_rep::clear();
	FixedComplex16 c = a - b;

	it=sc_dt::scfx_rep::zeroes.find(sc_dt::key(16,1)); //Access minMax zeroes for <16,1>
	BOOST_CHECK(it->second.getMin() == 0);// 0 100000...
	BOOST_CHECK(it->second.getMax() == 15);// 0 100000...
}

CSIM_TEST_CASE(Addition_Warning)
{
	std::cerr.setstate(std::ios_base::failbit);//mute cerr
    sc_dt::scfx_rep::warningLevel = 2;//0 - disable, 1 = warning, 2 = throw on warning
    int exceptionNum = 0;
    FixedPoint <5, 5> z(0);

    try {
    	FixedPoint <4, 4>  v(0);
        FixedPoint <4, 4>  w(0);
        z = (v + w); //No warning
    }
    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 0);//No throw

    try {
	FixedPoint <6, 3>  v2(0);
	FixedPoint <4, 4>  w2(0);
	z = (v2 + w2);//Precision warning
    }
    catch (int e){
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 1);//Throw precision warning
    exceptionNum = 0;

    try {
        FixedPoint <5, 5>  v3(0);
        FixedPoint <4, 4>  w3(0);
        z = (v3 + w3);//Range warning
    }

    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 2);//Throw range warning
    exceptionNum = 0;


    try {
        FixedPoint <6, 5>  v4(0);
        FixedPoint <6, 5>  w4(0);
        z = (v4 + w4);//Precision and range warning
    }
    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 3);//Throw precision and range warning
    std::cerr.clear();//unmute cerr
}

CSIM_TEST_CASE(Subtraction_Warning)
{
	std::cerr.setstate(std::ios_base::failbit);
    sc_dt::scfx_rep::warningLevel = 2;//0 - disable, 1 = warning, 2 = throw on warning
    int exceptionNum = 0;
    FixedPoint <5, 5> z;

    try {
        FixedPoint <4, 4>  v(0);
        FixedPoint <4, 4>  w(0);
        z = (v - w); //No warning
    }
    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 0);
    exceptionNum = 0;

    try {
        FixedPoint <6, 3>  v2(0);
        FixedPoint <4, 4>  w2(0);
        z = (v2 - w2);//Precision warning
    }
    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 1);
    exceptionNum = 0;

    try {
        FixedPoint <5, 5>  v3(0);
        FixedPoint <4, 4>  w3(0);
        z = (v3 - w3);//Range warning
    }
    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 2);
    exceptionNum = 0;

    try {
        FixedPoint <6, 5>  v4(0);
        FixedPoint <6, 5>  w4(0);
        z = (v4 - w4);//Precision and range warning
    }
    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 3);
    exceptionNum = 0;

    std::cerr.clear();
}

CSIM_TEST_CASE(Multiplication_Warning)
{
	std::cerr.setstate(std::ios_base::failbit);
    sc_dt::scfx_rep::warningLevel = 2;//0 - disable, 1 = warning, 2 = throw on warning
    int exceptionNum = 0;
    FixedPoint <8, 8> z;

	try {
	    FixedPoint <4, 4>  v(0);
	    FixedPoint <4, 4>  w(0);
	    z = (v * w); //No warning
    }
    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 0);
    exceptionNum = 0;

    try {
        FixedPoint <4, 3>  v2(0);
        FixedPoint <4, 4>  w2(0);
        z = (v2 * w2);//Precision warning
    }
    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 1);
    exceptionNum = 0;

    try {
        FixedPoint <5, 5>  v3(0);
        FixedPoint <4, 4>  w3(0);
        z = (v3 * w3);//Range warning
    }
    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 2);
    exceptionNum = 0;

    try {
        FixedPoint <5, 5>  v4(0);
        FixedPoint <5, 4>  w4(0);
        z = (v4 * w4);//Precision and range warning
    }
    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 3);
    exceptionNum = 0;

    std::cerr.clear();
}

CSIM_TEST_CASE(Division_Warning)
{
	std::cerr.setstate(std::ios_base::failbit);
    sc_dt::scfx_rep::warningLevel = 2;//0 - disable, 1 = warning, 2 = throw on warning
    int exceptionNum = 0;
	FixedPoint <4, 4> z;

    try {

        FixedPoint <4, 4>  v2(1);
        FixedPoint <4, 4>  w2(1);
        z = (v2 / w2);//Precision warning

    }
    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 1);
    exceptionNum = 0;

    try {
        FixedPoint <5, 5>  v4(1);
        FixedPoint <4, 4>  w4(1);
        z = (v4 / w4);//Precision and range warning
    }
    catch (int e) {
        exceptionNum = e;
    }

    BOOST_CHECK(exceptionNum == 3);
    exceptionNum = 0;

    std::cerr.clear(); //Reactivate cerr
}

CSIM_TEST_CASE(minMax_FixedPoint)
{
	  sc_dt::scfx_rep::warningLevel = 0;//0 - disable, 1 = warning, 2 = throw on warning
	sc_dt::scfx_rep::clear();
	FixedPoint<5,5> v(4);
	FixedPoint<5,5> w(5);
	FixedPoint<10,8> z = v+w;

	std::map<sc_dt::key,sc_dt::minMax>::iterator it=sc_dt::scfx_rep::values.find(sc_dt::key(5,5));
	BOOST_CHECK(it->second.getMin() == 4);
	BOOST_CHECK(it->second.getMax() == 5);

	it=sc_dt::scfx_rep::values.find(sc_dt::key(10,8));
	BOOST_CHECK(it->second.getMin() == 36); //9 * 2 * 2
	BOOST_CHECK(it->second.getMax() == 36);
	sc_dt::scfx_rep::clear();

	FixedPoint<6,3> v2(.25);
	FixedPoint<6,3> w2(.5);
	FixedPoint<10,5> z2 = v2+w2;
	it=sc_dt::scfx_rep::values.find(sc_dt::key(6,3));
	BOOST_CHECK(it->second.getMin() == 2); //.25 * 2 * 2 * 2
	BOOST_CHECK(it->second.getMax() == 4); //.5 * 2 ^ 3
	it=sc_dt::scfx_rep::values.find(sc_dt::key(10,5));
	BOOST_CHECK(it->second.getMin() == 24); // .75 * 2^5
	BOOST_CHECK(it->second.getMax() == 24);
}

CSIM_TEST_CASE(minMax_FixedComplex)
{
	sc_dt::scfx_rep::clear();
	FixedComplex16 v(.4,.3);
	FixedComplex16 w(.5,.2);
	std::map<sc_dt::key,sc_dt::minMax>::iterator it=sc_dt::scfx_rep::values.find(sc_dt::key(16,1));
	BOOST_CHECK(it->second.getMin() == (6553));// floor(.2 * 2 ^ 15)
	BOOST_CHECK(it->second.getMax() == (16384));// 2 * 2 ^ 15

	FixedComplex16 z(0);
	z = v+w;
	it=sc_dt::scfx_rep::values.find(sc_dt::key(16,1));
	BOOST_CHECK(it->second.getMin() == (0));// floor(.2 * 2 ^ 15)
	BOOST_CHECK(it->second.getMax() == (29491));// 2 * 2 ^ 15

}

CSIM_TEST_CASE(Top_Level_Value)
{
	FixedPoint<5,5> x(3);
	FixedPoint<5,4> y(2);
	FixedPoint<10,5> z = x + y;

	BOOST_CHECK(x.int64Val == 3);// 3 * 2^0
	BOOST_CHECK(x.binaryVal == "00011.");
	BOOST_CHECK(y.int64Val == 4);// 2 * (2^(5-4))
	BOOST_CHECK(y.binaryVal == "0010.0");
	BOOST_CHECK(z.int64Val == 160); // (3 + 2) * 2 ^(10-5)
	BOOST_CHECK(z.binaryVal == "00101.00000");

	z = 0;
	BOOST_CHECK(z.int64Val == 0); // (0) * 2 ^(10-5)
	BOOST_CHECK(z.binaryVal == "00000.00000");


}

CSIM_TEST_CASE(Conversion)
{
	FixedPoint<32,17> x = 1;
	FixedPoint<64,49> y = x;

	BOOST_CHECK(x.int64Val == 32768 );// 3 * 2^0
	BOOST_CHECK(x.binaryVal == "00000000000000001.000000000000000");
	BOOST_CHECK(y.int64Val == 32768);// 2 * (2^(5-4))
	BOOST_CHECK(y.binaryVal == "0000000000000000000000000000000000000000000000001.000000000000000");

}
#endif

CSIM_TEST_SUITE_END()
