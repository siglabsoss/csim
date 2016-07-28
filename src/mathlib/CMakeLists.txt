set(SIGMATH_SOURCES
	radio_physics.cpp
    cordic.cpp
    complex_gaussian_noise.cpp
    )

add_library(sigmath ${SIGMATH_SOURCES})
