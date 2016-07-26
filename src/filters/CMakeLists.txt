set(SIGFILTERS_SOURCES
    cic.cpp
    fixedfir.cpp
    fixediir.cpp
	fixedfft.cpp
	floatfft.cpp
    stitcher.cpp)

add_library(sigfilters ${SIGFILTERS_SOURCES})

