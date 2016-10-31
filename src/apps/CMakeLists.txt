########################### Analysis Tools ###########################
add_executable(ldpc_analysis ldpc_analysis.cpp)
target_link_libraries(ldpc_analysis ${Boost_LIBRARIES} sigcore sigtypes sigmath sigutils sigfilters sigprobes jsoncpp ${SystemC_LIBRARIES} pthread rt zmq)

add_executable(twiddles print_twiddle.cpp)
target_link_libraries(twiddles sigfilters sigtypes)

add_executable(complex_gaussian complex_gaussian_generator.cpp)
target_link_libraries(complex_gaussian sigmath)

add_executable(fft_runner fft_runner.cpp)
target_link_libraries(fft_runner sigfilters sigtypes)
