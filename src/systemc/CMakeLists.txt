############################ Anurag SYSTEMC: fft  ###########################

set(SYSTEMC_SRCS
    driver.cpp
    monitor.cpp
    memory.cpp
    syscfft.cpp
    twiddler.cpp
    butterfly_two_pt.cpp
    complex_alu.cpp
)

add_executable(fixedfft_main ${SYSTEMC_SRCS} fixedfft_main.cpp)
target_link_libraries(fixedfft_main ${Boost_LIBRARIES} ${ZEROMQ_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${SystemC_LIBRARIES} pthread  sigfilters)
add_custom_target(runfixedfft_main COMMAND fixedfft_main DEPENDS fixedfft_main WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

