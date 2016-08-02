set(SIGFILTERS_SOURCES
    cic.cpp
    fixedfir.cpp
    fixediir.cpp
	fixedfft.cpp
	fixedifft.cpp
	floatfft.cpp
    stitcher.cpp
    xcorrelator.cpp
    )

add_library(sigfilters ${SIGFILTERS_SOURCES})

