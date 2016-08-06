########################### MULTIPLY ###########################
add_executable(multiply multiply.cpp)
target_link_libraries(multiply ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters)

########################### FFT ###########################
add_executable(FFT testfft.cpp)
target_link_libraries(FFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils sigcore)

########################### HighResourceFFT ###########################
add_executable(HighResourceFFT testfft.cpp)
target_link_libraries(HighResourceFFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils sigcore)

########################### LowResourceFFT ###########################
add_executable(LowResourceFFT testfft.cpp)
target_link_libraries(LowResourceFFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils sigcore)

########################### HighResourceIFFT ###########################
add_executable(HighResourceIFFT testifft.cpp)
target_link_libraries(IFFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils sigcore)

########################### LowResourceIFFT ###########################
add_executable(LowResourceIFFT testifft.cpp)
target_link_libraries(IFFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils sigcore)


########################### IFFT ###########################
add_executable(IFFT testifft.cpp)
target_link_libraries(IFFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils sigcore)


########################### ALU ###########################
add_executable(ALU aluverifier.cpp)
target_link_libraries(ALU ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils)

########################### ALU ###########################
add_executable(ALU2 aluverifier.cpp)
target_link_libraries(ALU2 ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils )

