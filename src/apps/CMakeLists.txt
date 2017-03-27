########################### Analysis Tools ###########################
add_executable(twiddles print_twiddle.cpp)
target_link_libraries(twiddles sigfilters sigtypes)

add_executable(complex_gaussian complex_gaussian_generator.cpp)
target_link_libraries(complex_gaussian sigmath)

add_executable(fft_runner fft_runner.cpp)
target_link_libraries(fft_runner sigfilters sigtypes sigutils)

add_executable(filter_loopback filter_loopback.cpp)
target_link_libraries(filter_loopback sigfilters sigcore sigmath sigtypes sigutils)

add_executable(complex_multiplier complex_multiplier.cpp)
target_link_libraries(complex_multiplier sigfilters sigcore sigmath sigtypes sigutils)

add_executable(ldpc_decode ldpc_decode.cpp)
target_link_libraries(ldpc_decode sigfilters sigcore sigmath sigtypes sigutils)

add_executable(timing_alignment timing_alignment.cpp)
target_link_libraries(timing_alignment sigfilters sigcore sigmath sigtypes sigutils)
