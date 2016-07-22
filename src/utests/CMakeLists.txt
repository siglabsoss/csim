

add_executable(test test.cpp)
target_link_libraries(test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${SystemC_LIBRARIES} pthread sigtypes)
#set_target_properties(test PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
add_custom_target(runtest COMMAND test DEPENDS test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

############################ TEST FIR ###########################
add_executable(testfir ${SYSTEM_UNDER_TEST_SOURCES} testfir.cpp)
target_link_libraries(testfir ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} sigtypes ${SystemC_LIBRARIES} pthread sigfilters)
#set_target_properties(testfir PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
add_custom_target(runtestfir COMMAND testfir DEPENDS testfir WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

########################### TEST IIR ###########################
add_executable(testiir ${SYSTEM_UNDER_TEST_SOURCES} testiir.cpp)
target_link_libraries(testiir ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} sigtypes ${SystemC_LIBRARIES} pthread sigfilters)
#set_target_properties(testiir PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
add_custom_target(runtestiir COMMAND testiir DEPENDS testiir WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

########################### TEST CIC ###########################
add_executable(testcic ${SYSTEM_UNDER_TEST_SOURCES} testcic.cpp)
target_link_libraries(testcic ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} sigtypes ${SystemC_LIBRARIES} pthread sigfilters)

########################### Test Stitcher ###########################
add_executable(teststitch ${SYSTEM_UNDER_TEST_SOURCES} teststitch.cpp)
target_link_libraries(teststitch ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters sigtypes sigmath)

########################### Test Radio ###########################
add_executable(testradio testradio.cpp)
target_link_libraries(testradio ${Boost_LIBRARIES} pthread sigcore sigfilters sigtypes ${SystemC_LIBRARIES} )

########################### CordicTester ###########################
add_executable(cordictester cordictester.cpp)
target_link_libraries(cordictester ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread  sigfilters sigtypes sigmath)
