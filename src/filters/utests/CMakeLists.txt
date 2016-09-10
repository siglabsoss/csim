add_executable(test_cic ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_cic.cpp)
target_link_libraries(test_cic ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_fir ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_fir.cpp)
target_link_libraries(test_fir ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_fixedfft ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_fixedfft.cpp)
target_link_libraries(test_fixedfft ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_fixedifft ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_fixedifft.cpp)
target_link_libraries(test_fixedifft ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_iir ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_iir.cpp)
target_link_libraries(test_iir ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_modulator ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_modulator.cpp)
target_link_libraries(test_modulator ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_stitcher ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_stitcher.cpp)
target_link_libraries(test_stitcher ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_xcorrelator ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_xcorrelator.cpp)
target_link_libraries(test_xcorrelator ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_zero_pad_interpolator ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_zero_pad_interpolator.cpp)
target_link_libraries(test_zero_pad_interpolator ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)

add_executable(test_ldpc ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_ldpc.cpp)
target_link_libraries(test_ldpc ${Boost_LIBRARIES} pthread sigfilters sigcore sigtypes sigmath sigutils ${SystemC_LIBRARIES} zmq)
