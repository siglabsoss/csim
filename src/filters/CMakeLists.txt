set(SIGFILTERS_SOURCES
    fixed_fir.cpp
    fixed_iir.cpp
    fixed_cic.cpp
    fixedfft.cpp  
    #stitcher.cpp
    xcorrelator.cpp
    modulator.cpp
    zero_pad_interpolator.cpp
    mixer.cpp
    )

add_library(sigfilters ${SIGFILTERS_SOURCES})

