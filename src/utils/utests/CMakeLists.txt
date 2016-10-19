add_executable(test_utils ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_utils.cpp)
target_link_libraries(test_utils ${Boost_LIBRARIES} sigutils)

add_executable(test_scrambler ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_scrambler.cpp)
target_link_libraries(test_scrambler ${Boost_LIBRARIES} sigutils)

