
set(DCAPP_DIR ${CMAKE_SOURCE_DIR}/src/DuneCat_App)
set(DCNET_DIR ${CMAKE_SOURCE_DIR}/src/DuneCat_Network)
add_executable(networking_tests tst_networkingtests.cpp
    ${DCNET_DIR}/dcstunclient.cpp ${DCNET_DIR}/dcstunclient.h
    ${DCAPP_DIR}/dctools.h ${DCAPP_DIR}/dctools.cpp
)
add_test(NAME networking_tests COMMAND networking_tests)

target_include_directories(networking_tests PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(networking_tests PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(networking_tests PUBLIC ${CMAKE_SOURCE_DIR}/src/DuneCat_App)
target_link_libraries(networking_tests PRIVATE qt_libs Qt6::Test)

