add_executable(test_slfixedpoint ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_slfixedpoint.cpp)
target_link_libraries(test_slfixedpoint sigtypes)
