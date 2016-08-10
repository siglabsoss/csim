#include <test/unit_test.hpp>

#include <core/parameters.hpp>
#include <cfloat>


PARAM_DEFINE(DUMMY_INT1, 1234l);
PARAM_DEFINE(DUMMY_INT2, 9876l);
PARAM_DEFINE(DUMMY_INT3, 5432l);
PARAM_DEFINE(DUMMY_INT4, 8888l);

PARAM_DEFINE(DUMMY_DOUBLE1, 3.14);
PARAM_DEFINE(DUMMY_DOUBLE2, 654.142464);
PARAM_DEFINE(DUMMY_DOUBLE3, 9443.41188777);
PARAM_DEFINE(DUMMY_DOUBLE4, 6972.2168684722);

CSIM_TEST_SUITE_BEGIN(ParameterInterface)

CSIM_TEST_CASE(READ_INTEGER)
{
    int64_t value = 0;
    param_get("DUMMY_INT1", value);
    BOOST_CHECK_EQUAL(value, 1234l);

    param_get("DUMMY_INT2", value);
    BOOST_CHECK_EQUAL(value, 9876l);

    param_get("DUMMY_INT3", value);
    BOOST_CHECK_EQUAL(value, 5432l);

    param_get("DUMMY_INT4", value);
    BOOST_CHECK_EQUAL(value, 8888l);
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
