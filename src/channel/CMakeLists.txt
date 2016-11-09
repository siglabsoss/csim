set(SIGCHANNEL_SOURCES
    pathloss.cpp
    sui.cpp
)

add_library(sigchannelobj OBJECT ${SIGCHANNEL_SOURCES})
add_library(sigchannel $<TARGET_OBJECTS:sigchannelobj>)
