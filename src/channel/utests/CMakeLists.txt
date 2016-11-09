add_executable(test_pathloss ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_pathloss.cpp)
target_link_libraries(test_pathloss sigchannel)

