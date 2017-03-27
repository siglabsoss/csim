set(SIGCORE_SOURCES
	filter_chain.cpp
	filter_chain_element.cpp
	radio_s.cpp
	radio_set.cpp
	sigworld.cpp
	logger.cpp
	publisher.cpp
	parameters.cpp
	filter_probe.cpp
)

add_library(sigcoreobj OBJECT ${SIGCORE_SOURCES})
add_library(sigcore $<TARGET_OBJECTS:sigcoreobj>)
target_link_libraries(sigcore sigutils zmq jsoncpp)
