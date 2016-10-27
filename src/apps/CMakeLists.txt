########################### Analysis Tools ###########################
add_executable(retrosim retrosim.cpp)
target_link_libraries(retrosim sigcore sigtypes sigmath sigutils sigfilters jsoncpp rt zmq)

add_executable(bertester ber_tester.cpp)
target_link_libraries(bertester sigcore sigtypes sigmath sigutils sigfilters jsoncpp rt zmq)

add_executable(ldpc_analysis ldpc_analysis.cpp)
target_link_libraries(ldpc_analysis ${Boost_LIBRARIES} sigcore sigtypes sigmath sigutils sigfilters sigprobes jsoncpp ${SystemC_LIBRARIES} pthread rt zmq)

add_executable(twiddles print_twiddle.cpp)
target_link_libraries(twiddles sigfilters sigtypes)

add_executable(complex_gaussian complex_gaussian_generator.cpp)
target_link_libraries(complex_gaussian sigmath)
