set(ALL_SOURCES

	fixedcomplex.cpp
	
	cordic.cpp
	Stitcher.cpp
	
)




########################### CordicTester ###########################
add_executable(cordictester ${ALL_SOURCES} cordictester.cpp)
target_link_libraries(cordictester ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread)

########################### Test Stitcher ###########################
add_executable(teststitch ${ALL_SOURCES} teststitch.cpp)
target_link_libraries(teststitch ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread)