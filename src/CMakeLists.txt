set(ALL_SOURCES
	osibase.cpp
	osiderived.cpp
	plotter.cpp
	fixedcomplex.cpp
	../3rd/jsoncpp.cpp
	fixedfir.cpp
	fixediir.cpp
	cic.cpp
	
)



########################### For target Davey ###########################
add_executable(davey ${ALL_SOURCES} main.cpp)
target_link_libraries(davey ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${SystemC_LIBRARIES} pthread)
add_custom_target(rundavey COMMAND davey DEPENDS davey WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


########################### TEST ###########################
add_executable(test ${ALL_SOURCES} test.cpp)
target_link_libraries(test ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${SystemC_LIBRARIES} pthread)
#set_target_properties(test PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
add_custom_target(runtest COMMAND test DEPENDS test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

########################### SYSTEMC ###########################
add_executable(sysc ${ALL_SOURCES} sysc.cpp)
target_link_libraries(sysc ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${SystemC_LIBRARIES} pthread)
add_custom_target(runsysc COMMAND sysc DEPENDS sysc WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


########################### CLIENT ###########################
add_executable(client ${ALL_SOURCES} client.cpp)
target_link_libraries(client ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread)
add_custom_target(runclient COMMAND client DEPENDS client WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


########################### MULTIPLY ###########################
add_executable(multiply ${ALL_SOURCES} multiply.cpp)
target_link_libraries(multiply ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread)

########################### TEST FIR ###########################
add_executable(testfir ${ALL_SOURCES} testfir.cpp)
target_link_libraries(testfir ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${SystemC_LIBRARIES} pthread)
#set_target_properties(testfir PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
add_custom_target(runtestfir COMMAND testfir DEPENDS testfir WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

########################### TEST IIR ###########################
add_executable(testiir ${ALL_SOURCES} testiir.cpp)
target_link_libraries(testiir ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${SystemC_LIBRARIES} pthread)
#set_target_properties(testiir PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
add_custom_target(runtestiir COMMAND testiir DEPENDS testiir WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

########################### TEST CIC ###########################
add_executable(testcic ${ALL_SOURCES} testcic.cpp)
target_link_libraries(testcic ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread)

########################### MULTIPLY ###########################
add_executable(aluverifier ${ALL_SOURCES} aluverifier.cpp)
target_link_libraries(aluverifier ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread)
