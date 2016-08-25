set(JSONCPP_SOURCES
	jsoncpp.cpp
)

add_library(jsoncppobj OBJECT ${JSONCPP_SOURCES})
add_library(jsoncpp $<TARGET_OBJECTS:jsoncppobj>)
