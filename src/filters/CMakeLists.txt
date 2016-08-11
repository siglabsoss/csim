set(SIGFILTERS_SOURCES
    cic.cpp
    fixed_fir.cpp
    fixediir.cpp
    fixedfft.cpp
    fixedifft.cpp
    floatfft.cpp
    #stitcher.cpp
    xcorrelator.cpp
    modulator.cpp
    zero_pad_interpolator.cpp
    mixer.cpp
    )

add_library(sigfilters ${SIGFILTERS_SOURCES})

