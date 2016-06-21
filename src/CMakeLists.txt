set(ALL_SOURCES
	sigmath.cpp
	osibase.cpp
	osiderived.cpp
)



# For target Davey
add_executable(davey ${ALL_SOURCES} main.cpp)
target_link_libraries(davey ${Boost_LIBRARIES} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(test ${ALL_SOURCES} test.cpp)
target_link_libraries(test ${Boost_LIBRARIES} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
set_target_properties(test PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")

add_custom_target(rundavey COMMAND davey DEPENDS davey WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
