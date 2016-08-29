########################### Simulator ###########################
add_executable(retrosim retrosim.cpp)
target_link_libraries(retrosim ${Boost_LIBRARIES} sigcore sigtypes sigmath sigutils sigfilters sigprobes jsoncpp ${SystemC_LIBRARIES} pthread rt zmq)

add_executable(bertester ber_tester.cpp)
target_link_libraries(bertester ${Boost_LIBRARIES} sigcore sigtypes sigmath sigutils sigfilters sigprobes jsoncpp ${SystemC_LIBRARIES} pthread rt zmq)