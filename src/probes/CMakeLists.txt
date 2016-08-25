set(SIGPROBES_SOURCES
	level_trigger.cpp
    )

add_library(sigprobesobj OBJECT ${SIGPROBES_SOURCES})
add_library(sigprobes $<TARGET_OBJECTS:sigprobesobj>)
