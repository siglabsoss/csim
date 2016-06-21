cmake_minimum_required(VERSION 2.8)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

set(BOOST_INCLUDEDIR /usr/include)
find_package(Boost)

project(davey)
set(CMAKE_BUILD_TYPE debug)




include_directories(
	${Boost_INCLUDE_DIRS}
)

link_directories(
	${Boost_LIBRARY_DIRS}
)





add_subdirectory(src)
