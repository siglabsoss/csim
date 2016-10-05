set(SIGTYPES_SOURCES
    filter_io.cpp
    fixedpoint.cpp
    fixedcomplex.cpp
    )

add_library(sigtypesobj OBJECT ${SIGTYPES_SOURCES})
add_library(sigtypes $<TARGET_OBJECTS:sigtypesobj>)

