set(SIGFILTERS_SOURCES
    fixed_fir.cpp
    fixed_iir.cpp
    fixed_cic.cpp
    stitcher.cpp
    modulator.cpp
    zero_pad_interpolator.cpp
    mixer.cpp
    decimator.cpp
    hard_demod.cpp
    biquad.cpp
    biquad_iir.cpp
    bfp_fft.cpp
    )

add_library(sigfiltersobj OBJECT ${SIGFILTERS_SOURCES})
add_library(sigfilters $<TARGET_OBJECTS:sigfiltersobj>)

