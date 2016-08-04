########################### MULTIPLY ###########################
add_executable(multiply multiply.cpp)
target_link_libraries(multiply ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters)

########################### FFT ###########################
add_executable(FFT testfft.cpp)
target_link_libraries(FFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils sigcore)

########################### IFFT ###########################
add_executable(IFFT testifft.cpp)
target_link_libraries(IFFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils sigcore)

########################### ALU ###########################
add_executable(ALU aluverifier.cpp)
target_link_libraries(ALU ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils)

########################### ALU ###########################
add_executable(ALU2 aluverifier.cpp)
target_link_libraries(ALU2 ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils )

########################### BitReverse ###########################
add_executable(bitReverse bitReverse.cpp)
target_link_libraries(bitReverse ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils )

