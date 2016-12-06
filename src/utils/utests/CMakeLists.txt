add_executable(test_utils ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_utils.cpp)
target_link_libraries(test_utils sigutils)

add_executable(test_scrambler ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_scrambler.cpp)
target_link_libraries(test_scrambler sigutils)

add_executable(test_frame_gen ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_frame_gen.cpp)
target_link_libraries(test_frame_gen sigutils sigfilters sigtypes)
