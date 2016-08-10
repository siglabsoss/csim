add_executable(test_fixedcomplex ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_fixedcomplex.cpp)
target_link_libraries(test_fixedcomplex ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_fixedpoint ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_fixedpoint.cpp)
target_link_libraries(test_fixedpoint ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)
