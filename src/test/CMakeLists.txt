
############################ Unit Tests ###########################

set(UNIT_TEST_SRCS $ENV{UNIT_TEST_SRCS})

add_executable(utests utests_main.cpp ${UNIT_TEST_SRCS})
#add_executable(utests utests_main.cpp ../filters/utests/test_fir.cpp)
target_link_libraries(utests ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

