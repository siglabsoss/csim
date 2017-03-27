add_executable(test_slfixedpoint ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_slfixedpoint.cpp)
target_link_libraries(test_slfixedpoint sigtypes)

add_executable(test_slfixedcomplex ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_slfixedcomplex.cpp)
target_link_libraries(test_slfixedcomplex sigtypes)

add_executable(test_circularbuffer ${CMAKE_SOURCE_DIR}/test/utests_main.cpp test_circularbuffer.cpp)
target_link_libraries(test_circularbuffer sigtypes)
