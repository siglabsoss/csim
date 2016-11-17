set(SIGFILTERS_SOURCES
    fixed_fir.cpp
    biquad.cpp
    biquad_iir.cpp
    fft.cpp
    ddc_duc/ddc.cpp
    ddc_duc/duc.cpp
    ddc_duc/ddc_utils.cpp
    mixer.cpp
    decimator.cpp
    mapper.cpp
    demapper.cpp
    nco.cpp
    noise_element.cpp
    ldpc_decoder.cpp
    ldpc_encode.cpp
    cyclic_prefix.cpp
    )

add_library(sigfiltersobj OBJECT ${SIGFILTERS_SOURCES})
add_library(sigfilters $<TARGET_OBJECTS:sigfiltersobj>)
target_link_libraries(sigfilters sigutils sigcore)
