########################### Simulator ###########################
add_executable(retrosim retrosim.cpp)
target_link_libraries(retrosim sigcore sigtypes sigmath sigutils sigfilters jsoncpp rt zmq)

add_executable(bertester ber_tester.cpp)
target_link_libraries(bertester sigcore sigtypes sigmath sigutils sigfilters jsoncpp rt zmq)