set(SIGFILTERS_SOURCES
    fixed_cic.cpp
    fixed_fir.cpp
    fixed_iir.cpp
    fixedfft.cpp
    fixedifft.cpp
    floatfft.cpp
    #stitcher.cpp
    xcorrelator.cpp
    modulator.cpp
    zero_pad_interpolator.cpp
    mixer.cpp
    decimator.cpp
    hard_demod.cpp
    )

add_library(sigfiltersobj OBJECT ${SIGFILTERS_SOURCES})
add_library(sigfilters $<TARGET_OBJECTS:sigfiltersobj>)

