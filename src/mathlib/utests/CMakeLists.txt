add_executable(test_radio_physics ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_radio_physics.cpp)
target_link_libraries(test_radio_physics sigcore sigmath sigutils)
