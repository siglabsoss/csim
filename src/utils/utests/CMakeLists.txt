add_executable(test_utils ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_utils.cpp)
target_link_libraries(test_utils ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)
