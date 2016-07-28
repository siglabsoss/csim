########################### MULTIPLY ###########################
add_executable(multiply multiply.cpp)
target_link_libraries(multiply ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters)

########################### FFT ###########################
#add_executable(testfft testfft.cpp)
#target_link_libraries(testfft ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath sigutils)
