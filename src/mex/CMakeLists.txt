set(LIBCSIM_SOURCES
    $<TARGET_OBJECTS:sigcoreobj>
    $<TARGET_OBJECTS:sigfiltersobj>
    $<TARGET_OBJECTS:sigmathobj>
    $<TARGET_OBJECTS:sigprobesobj>
    $<TARGET_OBJECTS:sigtypesobj>
    $<TARGET_OBJECTS:sigutilsobj>
    $<TARGET_OBJECTS:jsoncppobj>
    mex_utils.c
    csim.cpp
   )


add_library(csim SHARED ${LIBCSIM_SOURCES})
install(TARGETS csim DESTINATION lib)
