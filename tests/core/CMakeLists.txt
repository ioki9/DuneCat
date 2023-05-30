set(DCCORE_DIR ${CMAKE_SOURCE_DIR}/src/DuneCat_Core)
set(DCAPP_DIR ${CMAKE_SOURCE_DIR}/src/DuneCat_App)
set(DCNET_DIR ${CMAKE_SOURCE_DIR}/src/DuneCat_Network)
set(SOURCE_LIST tst_coretests.cpp
    ${DCCORE_DIR}/dcprocesstracker.h ${DCAPP_DIR}/dctools.h ${DCAPP_DIR}/dctools.cpp )
if(WIN32)
    list(APPEND SOURCE_LIST ${DCCORE_DIR}/windows/dcprocesstracker.cpp
        ${DCCORE_DIR}/windows/wmieventsink.h ${DCCORE_DIR}/windows/wmieventsink.cpp
        ${DCCORE_DIR}/windows/wmiclient.h ${DCCORE_DIR}/windows/wmiclient.cpp)
endif()

add_executable(core_tests ${SOURCE_LIST}
)
add_test(NAME core_tests COMMAND $<TARGET_FILE:core_tests>)

target_include_directories(core_tests PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(core_tests PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(core_tests PUBLIC ${CMAKE_SOURCE_DIR}/src/DuneCat_App)
target_include_directories(core_tests PUBLIC ${CMAKE_SOURCE_DIR}/src/DuneCat_Core)
target_link_libraries(core_tests PRIVATE qt_libs Qt6::Test)

install(TARGETS core_tests
    RUNTIME DESTINATION "tests"
    BUNDLE DESTINATION "tests"
    LIBRARY DESTINATION "tests"
)
