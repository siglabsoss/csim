set(SIGFILTERS_SOURCES
    fixed_cic.cpp
    fixed_fir.cpp
    fixed_iir.cpp
    fixedfft.cpp
    oldfixedfft.cpp
    fixedifft.cpp
    floatfft.cpp
    #stitcher.cpp
    xcorrelator.cpp
    modulator.cpp
    zero_pad_interpolator.cpp
    mixer.cpp
    )

add_library(sigfilters ${SIGFILTERS_SOURCES})

