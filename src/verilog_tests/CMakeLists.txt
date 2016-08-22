########################### MULTIPLY ###########################
add_executable(multiply multiply.cpp)
target_link_libraries(multiply ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters)

########################### FFT ###########################
add_executable(FFT fft_verifier.cpp)
target_link_libraries(FFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY}  sigfilters sigtypes sigmath sigutils sigcore ${SystemC_LIBRARIES} pthread)

########################### HighResourceFFT ###########################
add_executable(HighResourceFFT fft_verifier.cpp)
target_link_libraries(HighResourceFFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} sigfilters sigtypes sigmath sigutils sigcore ${SystemC_LIBRARIES} pthread)

########################### LowResourceFFT ###########################
add_executable(LowResourceFFT fft_verifier.cpp)
target_link_libraries(LowResourceFFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} sigfilters sigtypes sigmath sigutils sigcore ${SystemC_LIBRARIES} pthread)

########################### HighResourceIFFT ###########################
add_executable(HighResourceIFFT ifft_verifier.cpp)
target_link_libraries(HighResourceIFFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} sigfilters sigtypes sigmath sigutils sigcore ${SystemC_LIBRARIES} pthread)

########################### LowResourceIFFT ###########################
add_executable(LowResourceIFFT ifft_verifier.cpp)
target_link_libraries(LowResourceIFFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY}  sigfilters sigtypes sigmath sigutils sigcore ${SystemC_LIBRARIES} pthread)

########################### IFFT ###########################
add_executable(IFFT ifft_verifier.cpp)
target_link_libraries(IFFT ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} sigfilters sigtypes sigmath sigutils sigcore ${SystemC_LIBRARIES} pthread)

########################### ALU ###########################
add_executable(ALU alu_verifier.cpp)
target_link_libraries(ALU ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils)

########################### ALU ###########################
add_executable(ALU2 alu_verifier.cpp)
target_link_libraries(ALU2 ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils )

########################### FIR ###########################
add_executable(FIR fir_verifier.cpp)
target_link_libraries(FIR ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY}  sigfilters sigtypes sigmath sigutils sigcore ${SystemC_LIBRARIES} pthread)

########################### IIR ###########################
add_executable(IIR iir_verifier.cpp)
target_link_libraries(IIR ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY}  sigfilters sigtypes sigmath sigutils sigcore ${SystemC_LIBRARIES} pthread)

########################### CIC ###########################
add_executable(CIC cic_verifier.cpp)
target_link_libraries(CIC ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY}  sigfilters sigtypes sigmath sigutils sigcore ${SystemC_LIBRARIES} pthread)
