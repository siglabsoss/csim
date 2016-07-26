########################### Simulator ###########################
add_executable(retrosim retrosim.cpp)
target_link_libraries(retrosim ${Boost_LIBRARIES} sigcore sigfilters sigtypes sigmath ${SystemC_LIBRARIES} pthread rt zmq)