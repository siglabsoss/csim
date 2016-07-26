########################### MULTIPLY ###########################
add_executable(multiply multiply.cpp)
target_link_libraries(multiply ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters)

########################### FFT ###########################
add_executable(FFT testfft.cpp)
target_link_libraries(FFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils)

########################### ALU ###########################
add_executable(ALU aluverifier.cpp)
target_link_libraries(ALU ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils)


add_custom_target(verilog_tests COMMAND ALU 1 DEPENDS ALU COMMAND ALU 2 DEPENDS ALU WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})