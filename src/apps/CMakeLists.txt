########################### Simulator ###########################
add_executable(retrosim retrosim.cpp)
target_link_libraries(retrosim ${Boost_LIBRARIES} sigcore sigtypes sigmath sigutils sigfilters sigprobes jsoncpp ${SystemC_LIBRARIES} pthread rt zmq)

add_executable(bertester ber_tester.cpp)
target_link_libraries(bertester ${Boost_LIBRARIES} sigcore sigtypes sigmath sigutils sigfilters sigprobes jsoncpp ${SystemC_LIBRARIES} pthread rt zmq)

add_executable(bertester1 ber_tester1.cpp)
target_link_libraries(bertester1 ${Boost_LIBRARIES} sigcore sigtypes sigmath sigutils sigfilters sigprobes jsoncpp ${SystemC_LIBRARIES} pthread rt zmq)

add_executable(bertester2 ber_tester2.cpp)
target_link_libraries(bertester2 ${Boost_LIBRARIES} sigcore sigtypes sigmath sigutils sigfilters sigprobes jsoncpp ${SystemC_LIBRARIES} pthread rt zmq)
