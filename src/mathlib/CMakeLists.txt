set(SIGMATH_SOURCES
	radio_physics.cpp
    complex_gaussian_noise.cpp
    )

add_library(sigmathobj OBJECT ${SIGMATH_SOURCES})
add_library(sigmath $<TARGET_OBJECTS:sigmathobj>)
