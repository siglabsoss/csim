set(SIGUTILS_SOURCES
    plotter.cpp
    utils.cpp)

add_library(sigutilsobj OBJECT ${SIGUTILS_SOURCES})
add_library(sigutils $<TARGET_OBJECTS:sigutilsobj>)
