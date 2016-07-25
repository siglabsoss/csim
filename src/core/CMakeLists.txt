set(SIGCORE_SOURCES
	filter_chain.cpp
	filter_chain_element.cpp
	radio_s.cpp
	radio_set.cpp
	sigworld.cpp
	logger.cpp
)

add_library(sigcore ${SIGCORE_SOURCES})
