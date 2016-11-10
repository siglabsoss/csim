project(SIGLABS_C_SIM)
cmake_minimum_required(VERSION 2.8)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../)  # this allows custom "cmake find" files to be located in the current directory

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -Wall -Werror -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -fPIC -Wall -Werror")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(C_SIM_SOURCE_ROOT ${CMAKE_SOURCE_DIR})
set(MATLAB_INCLUDE_PATH "/usr/local/MATLAB/R2016a/extern/include/")

find_package(Boost)

# --- 0MQ ---   https://www.mail-archive.com/zeromq-dev@lists.zeromq.org/msg17331.html http://pastebin.com/fLsm9SYf http://pastebin.com/sSJJVw6u

set( ZEROMQ_FIND_REQUIRED TRUE )
 
### ZeroMQ ###
 
find_path( ZEROMQ_INCLUDE_DIR NAMES zmq.h PATHS /usr/include/ /usr/local/include/ )
find_library( ZEROMQ_LIBRARY NAMES zmq PATHS /usr/lib /usr/local/lib )
 
if( ZEROMQ_INCLUDE_DIR AND ZEROMQ_LIBRARY )
    set( ZEROMQ_FOUND TRUE )
endif( ZEROMQ_INCLUDE_DIR AND ZEROMQ_LIBRARY )
 
 
if( ZEROMQ_FOUND )
    
    message( STATUS "Found ZeroMQ:" )
    message( STATUS "  (Headers)       ${ZEROMQ_INCLUDE_DIR}" )
    message( STATUS "  (Library)       ${ZEROMQ_LIBRARY}" )
   
else( ZEROMQ_FOUND )
    if( ZEROMQ_FIND_REQUIRED )
        message( FATAL_ERROR "Could not find ZeroMQ" )
    endif( ZEROMQ_FIND_REQUIRED )
endif( ZEROMQ_FOUND )


# --- End 0MQ ---


add_definitions(-DSC_DISABLE_API_VERSION_CHECK)
add_definitions(-DSC_INCLUDE_DYNAMIC_PROCESSES)

include_directories(
	${Boost_INCLUDE_DIRS}
	${C_SIM_SOURCE_ROOT}
        ${MATLAB_INCLUDE_PATH}
)

link_directories(
	${Boost_LIBRARY_DIRS}
)

option(BUILD_UTESTS "Option to build unit tests" false)

add_subdirectory(core)
add_subdirectory(filters)
add_subdirectory(mathlib)
add_subdirectory(probes)
add_subdirectory(types)
add_subdirectory(utils)
add_subdirectory(3rd)
add_subdirectory(channel)

if (EXISTS MATLAB_INCLUDE_PATH)
    add_subdirectory(mex)
endif()

if (BUILD_UTESTS)
    add_subdirectory(core/utests)
    add_subdirectory(filters/utests)
    add_subdirectory(mathlib/utests)
    add_subdirectory(types/utests)
    add_subdirectory(utils/utests)
    add_subdirectory(channel/utests)
else()
    add_subdirectory(apps)
endif()

