add_executable(test_fixedpoint ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_fixedpoint.cpp)
target_link_libraries(test_fixedpoint ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_slfixedpoint ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_slfixedpoint.cpp)
target_link_libraries(test_slfixedpoint ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)
