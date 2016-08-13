add_executable(test_filter_probe ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_filter_probe.cpp)
target_link_libraries(test_filter_probe ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils jsoncpp ${SystemC_LIBRARIES} zmq)

add_executable(test_parameters ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_parameters.cpp)
target_link_libraries(test_parameters ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_radio ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_radio.cpp)
target_link_libraries(test_radio ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)
