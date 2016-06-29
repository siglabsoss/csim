PROJECT(SIGLABS_C_CIM)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})  # this allows custom "cmake find" files to be located in the current directory


set(BOOST_INCLUDEDIR /usr/include)
find_package(Boost)



# --- 0MQ ---   https://www.mail-archive.com/zeromq-dev@lists.zeromq.org/msg17331.html http://pastebin.com/fLsm9SYf http://pastebin.com/sSJJVw6u

SET ( ZEROMQ_FIND_REQUIRED TRUE )
 
### ZeroMQ ###
 
FIND_PATH ( ZEROMQ_INCLUDE_DIR NAMES zmq.h PATHS /usr/include/ /usr/local/include/ )
FIND_LIBRARY ( ZEROMQ_LIBRARY NAMES zmq PATHS /usr/lib /usr/local/lib )
 
IF ( ZEROMQ_INCLUDE_DIR AND ZEROMQ_LIBRARY )
    SET ( ZEROMQ_FOUND TRUE )
ENDIF ( ZEROMQ_INCLUDE_DIR AND ZEROMQ_LIBRARY )
 
 
IF ( ZEROMQ_FOUND )
    
    MESSAGE ( STATUS "Found ZeroMQ:" )
    MESSAGE ( STATUS "  (Headers)       ${ZEROMQ_INCLUDE_DIR}" )
    MESSAGE ( STATUS "  (Library)       ${ZEROMQ_LIBRARY}" )
   
ELSE ( ZEROMQ_FOUND )
    IF ( ZEROMQ_FIND_REQUIRED )
        MESSAGE ( FATAL_ERROR "Could not find ZeroMQ" )
    ENDIF ( ZEROMQ_FIND_REQUIRED )
ENDIF ( ZEROMQ_FOUND )


# --- End 0MQ ---



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
	${CMAKE_SOURCE_DIR}/3rd/json
	${CMAKE_SOURCE_DIR}/3rd
)

link_directories(
	${Boost_LIBRARY_DIRS}
)



add_definitions(-DSC_DISABLE_API_VERSION_CHECK)
add_definitions(-DSC_INCLUDE_DYNAMIC_PROCESSES)

add_subdirectory(src)
