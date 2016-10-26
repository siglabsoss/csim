
add_executable(test_fir ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_fir.cpp)
target_link_libraries(test_fir ${Boost_LIBRARIES} sigfilters sigcore sigtypes sigutils)

add_executable(test_fixedfft ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_fixedfft.cpp)
target_link_libraries(test_fixedfft ${Boost_LIBRARIES} sigfilters sigcore sigtypes sigutils)

add_executable(test_biquad ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_biquad.cpp)
target_link_libraries(test_biquad ${Boost_LIBRARIES} sigfilters sigcore sigtypes sigutils)

add_executable(test_ddc ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_ddc.cpp)
target_link_libraries(test_ddc ${Boost_LIBRARIES} sigfilters sigcore sigtypes sigutils)

add_executable(test_mapper ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_mapper.cpp)
target_link_libraries(test_mapper ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils jsoncpp ${SystemC_LIBRARIES} zmq)

add_executable(test_demapper ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_demapper.cpp)
target_link_libraries(test_demapper ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils jsoncpp ${SystemC_LIBRARIES} zmq)

#Disabling CIC tests until it can pass with the new quantization / overflow modes
#add_executable(test_cic ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_cic.cpp)
#target_link_libraries(test_cic ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

#add_executable(test_stitcher ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_stitcher.cpp)
#target_link_libraries(test_stitcher ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

#add_executable(test_xcorrelator ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_xcorrelator.cpp)
#target_link_libraries(test_xcorrelator ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

#add_executable(test_zero_pad_interpolator ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_zero_pad_interpolator.cpp)
#target_link_libraries(test_zero_pad_interpolator ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_ldpc ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_ldpc.cpp)
target_link_libraries(test_ldpc ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)
