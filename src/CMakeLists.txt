include_directories(
	${Boost_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/3rd
	${C_SIM_SOURCE_ROOT}
)

link_directories(
	${Boost_LIBRARY_DIRS}
)

add_subdirectory(core)
add_subdirectory(filters)
add_subdirectory(mathlib)
add_subdirectory(types)
add_subdirectory(utils)
add_subdirectory(utests)
add_subdirectory(verilog_tests)
add_subdirectory(systemc)

