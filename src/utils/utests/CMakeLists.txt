add_executable(test_utils ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_utils.cpp)
target_link_libraries(test_utils sigutils)

add_executable(test_mcs ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_mcs.cpp)
target_link_libraries(test_mcs sigutils sigfilters sigtypes)

add_executable(test_timer ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_timer.cpp)
target_link_libraries(test_timer sigutils sigfilters sigtypes)
