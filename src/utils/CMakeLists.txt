set(SIGUTILS_SOURCES
    plotter.cpp
    radio_utils.cpp
    scrambler.cpp
    mcs.cpp
    timer.cpp
    filter_timer.cpp
    utils.cpp)

add_library(sigutilsobj OBJECT ${SIGUTILS_SOURCES})
add_library(sigutils $<TARGET_OBJECTS:sigutilsobj>)
