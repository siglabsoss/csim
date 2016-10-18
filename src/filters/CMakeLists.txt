set(SIGFILTERS_SOURCES
    fixed_fir.cpp
    biquad.cpp
    biquad_iir.cpp
    bfp_fft.cpp
    ddc.cpp
    duc.cpp
    mixer.cpp
    decimator.cpp
    mapper.cpp
    hard_demod.cpp
    nco.cpp
    )

add_library(sigfiltersobj OBJECT ${SIGFILTERS_SOURCES})
add_library(sigfilters $<TARGET_OBJECTS:sigfiltersobj>)

