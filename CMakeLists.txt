cmake_minimum_required(VERSION 2.8)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})  # this allows custom "cmake find" files to be located in the current directory


set(BOOST_INCLUDEDIR /usr/include)
find_package(Boost)

FIND_PACKAGE(SystemC)
if(SystemC_FOUND)
    include_directories(${SystemC_INCLUDE_DIRS})
    link_directories(${SystemC_LIBRARIES})
else()
    message( FATAL_ERROR "SystemC library not found." )
endif()

project(davey)
set(CMAKE_BUILD_TYPE debug)




include_directories(
	${Boost_INCLUDE_DIRS}
)

link_directories(
	${Boost_LIBRARY_DIRS}
)



add_definitions(-DSC_DISABLE_API_VERSION_CHECK)

add_subdirectory(src)
