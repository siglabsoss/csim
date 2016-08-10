add_executable(test_cordic ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_cordic.cpp)
target_link_libraries(test_cordic ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_radio_physics ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_radio_physics.cpp)
target_link_libraries(test_radio_physics ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)
