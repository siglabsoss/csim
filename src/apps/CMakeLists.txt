########################### Analysis Tools ###########################
add_executable(ldpc_analysis ldpc_analysis.cpp)
target_link_libraries(ldpc_analysis sigcore sigtypes sigmath sigutils sigfilters)

add_executable(twiddles print_twiddle.cpp)
target_link_libraries(twiddles sigfilters sigtypes)

add_executable(complex_gaussian complex_gaussian_generator.cpp)
target_link_libraries(complex_gaussian sigmath)

add_executable(fft_runner fft_runner.cpp)
target_link_libraries(fft_runner sigfilters sigtypes sigutils)

add_executable(filter_loopback filter_loopback.cpp)
target_link_libraries(filter_loopback sigfilters sigcore sigtypes)

