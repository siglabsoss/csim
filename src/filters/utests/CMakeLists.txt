
add_executable(test_fir ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_fir.cpp)
target_link_libraries(test_fir sigfilters sigtypes)

add_executable(test_fft ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_fft.cpp)
target_link_libraries(test_fft sigfilters sigtypes)

add_executable(test_biquad ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_biquad.cpp)
target_link_libraries(test_biquad sigfilters sigtypes)

add_executable(test_ddc ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_ddc.cpp)
target_link_libraries(test_ddc sigfilters sigtypes)

add_executable(test_mapper ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_mapper.cpp)
target_link_libraries(test_mapper sigfilters sigtypes)

add_executable(test_demapper ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_demapper.cpp)
target_link_libraries(test_demapper sigfilters sigtypes)

add_executable(test_ldpc ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_ldpc.cpp)
target_link_libraries(test_ldpc sigfilters sigtypes)

add_executable(test_cyclic_prefix ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_cyclic_prefix.cpp)
target_link_libraries(test_cyclic_prefix sigfilters sigtypes)

add_executable(test_puncture ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_puncture.cpp)
target_link_libraries(test_puncture sigfilters sigtypes)

add_executable(test_scrambler ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_scrambler.cpp)
target_link_libraries(test_scrambler sigfilters sigtypes)

add_executable(test_subcarrier_mapper ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_subcarrier_mapper.cpp)
target_link_libraries(test_subcarrier_mapper sigfilters sigtypes)
