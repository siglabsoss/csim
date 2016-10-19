#define BOOST_TEST_DYN_LINK
#define CSIM_TEST_SUITE_BEGIN           BOOST_AUTO_TEST_SUITE
#define CSIM_TEST_SUITE_END             BOOST_AUTO_TEST_SUITE_END
#define CSIM_TEST_CASE                  BOOST_AUTO_TEST_CASE
#define CSIM_TESX_CASE( test_name )     __attribute__((unused))   void test_name()

#include <boost/test/unit_test.hpp>

