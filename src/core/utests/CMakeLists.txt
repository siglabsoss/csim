add_executable(test_filter_probe ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_filter_probe.cpp)
target_link_libraries(test_filter_probe sigcore sigtypes sigmath sigutils)

add_executable(test_parameters ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_parameters.cpp)
target_link_libraries(test_parameters ${Boost_LIBRARIES} sigcore)

add_executable(test_radio ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_radio.cpp)
target_link_libraries(test_radio sigcore sigtypes)
