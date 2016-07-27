#include <test/unit_test.hpp>

#include <core/parameters.hpp>
#include <cfloat>


PARAM_DEFINE(DUMMY_INT1, 1234);
PARAM_DEFINE(DUMMY_INT2, 9876);
PARAM_DEFINE(DUMMY_INT3, 5432);
PARAM_DEFINE(DUMMY_INT4, 8888);

PARAM_DEFINE(DUMMY_DOUBLE1, 3.14);
PARAM_DEFINE(DUMMY_DOUBLE2, 654.142464);
PARAM_DEFINE(DUMMY_DOUBLE3, 9443.41188777);
PARAM_DEFINE(DUMMY_DOUBLE4, 6972.2168684722);

CSIM_TEST_SUITE_BEGIN(ParameterInterface)

CSIM_TEST_CASE(READ_INTEGER)
{
    int value = 0;
    param_get("DUMMY_INT1", value);
    BOOST_CHECK_EQUAL(value, 1234);

    param_get("DUMMY_INT2", value);
    BOOST_CHECK_EQUAL(value, 9876);

    param_get("DUMMY_INT3", value);
    BOOST_CHECK_EQUAL(value, 5432);

    param_get("DUMMY_INT4", value);
    BOOST_CHECK_EQUAL(value, 8888);
}

CSIM_TEST_CASE(READ_FLOATING_POINT)
{
    double dvalue = 0.0;
    param_get("DUMMY_DOUBLE1", dvalue);
    BOOST_CHECK_CLOSE(dvalue, 3.14, DBL_EPSILON);

    param_get("DUMMY_DOUBLE2", dvalue);
    BOOST_CHECK_CLOSE(dvalue, 654.142464, DBL_EPSILON);

    param_get("DUMMY_DOUBLE3", dvalue);
    BOOST_CHECK_CLOSE(dvalue, 9443.41188777, DBL_EPSILON);

    param_get("DUMMY_DOUBLE4", dvalue);
    BOOST_CHECK_CLOSE(dvalue, 6972.2168684722, DBL_EPSILON);
}

CSIM_TEST_SUITE_END()
