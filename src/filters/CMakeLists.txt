set(SIGFILTERS_SOURCES
    fixed_fir.cpp
    biquad.cpp
    biquad_iir.cpp
    fft.cpp
    ddc.cpp
    duc.cpp
    mixer.cpp
    decimator.cpp
    mapper.cpp
    hard_demapper.cpp
    nco.cpp
    soft_demapper.cpp
    noise_element.cpp
    ldpc_decoder.cpp
    ldpc_encode.cpp
    )

add_library(sigfiltersobj OBJECT ${SIGFILTERS_SOURCES})
add_library(sigfilters $<TARGET_OBJECTS:sigfiltersobj>)
target_link_libraries(sigfilters sigutils sigcore)
