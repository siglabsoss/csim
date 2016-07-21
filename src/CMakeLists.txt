FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

set(ALL_SOURCES
	osibase.cpp
	osiderived.cpp
	plotter.cpp
	fixedcomplex.cpp
	../3rd/jsoncpp.cpp
	

	cordic.cpp
	Stitcher.cpp
	
	floatfft.cpp
	fixedfft.cpp
	butterfly_two_pt.cpp
	complex_alu.cpp
	driver.cpp
	memory.cpp
	monitor.cpp
	syscfft.cpp
	twiddler.cpp
	utils.cpp
)

add_subdirectory(utests)
add_subdirectory(filters)
add_subdirectory(core)

########################### For target Davey ###########################
add_executable(davey ${ALL_SOURCES} main.cpp)
target_link_libraries(davey ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${SystemC_LIBRARIES} pthread sigfilters)
add_custom_target(rundavey COMMAND davey DEPENDS davey WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


########################### SYSTEMC ###########################
add_executable(sysc ${ALL_SOURCES} sysc.cpp)
target_link_libraries(sysc ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${SystemC_LIBRARIES} pthread  sigfilters)
add_custom_target(runsysc COMMAND sysc DEPENDS sysc WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


########################### CLIENT ###########################
add_executable(client ${ALL_SOURCES} client.cpp)
target_link_libraries(client ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread  sigfilters)
add_custom_target(runclient COMMAND client DEPENDS client WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


########################### MULTIPLY ###########################
add_executable(multiply ${ALL_SOURCES} multiply.cpp)
target_link_libraries(multiply ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread  sigfilters)

########################### MULTIPLY ###########################
add_executable(aluverifier ${ALL_SOURCES} aluverifier.cpp)
target_link_libraries(aluverifier ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread  sigfilters)

########################### CordicTester ###########################
add_executable(cordictester ${ALL_SOURCES} cordictester.cpp)
target_link_libraries(cordictester ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread  sigfilters)

########################### Anurag: FFT  ###########################
add_executable(pipefft ${ALL_SOURCES} pipefft.cpp)
target_link_libraries(pipefft ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread  sigfilters)
add_custom_target(runpipefft COMMAND pipefft DEPENDS pipefft WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


########################### Anurag SYSTEMC: fft  ###########################
add_executable(fixedfft_main ${ALL_SOURCES} fixedfft_main.cpp)
target_link_libraries(fixedfft_main ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${SystemC_LIBRARIES} pthread  sigfilters)
add_custom_target(runfixedfft_main COMMAND fixedfft_main DEPENDS fixedfft_main WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

########################### BEN FILTER ###########################
#add_executable(benfilter ${ALL_SOURCES} benfilter.cpp)
#target_link_libraries(benfilter ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ZEROMQ_LIBRARY} ${SystemC_LIBRARIES} pthread  sigfilters)
#add_custom_target(runbenfilter COMMAND benfilter DEPENDS benfilter WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

