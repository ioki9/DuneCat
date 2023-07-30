set(DCCORE_DIR ${CMAKE_SOURCE_DIR}/src/DuneCat_Core)
set(DCAPP_DIR ${CMAKE_SOURCE_DIR}/src/DuneCat_App)
set(DCNET_DIR ${CMAKE_SOURCE_DIR}/src/DuneCat_Network)
set(SOURCE_LIST tst_coretests.cpp
    ${DCCORE_DIR}/processtracker.h ${DCCORE_DIR}/tools.h ${DCCORE_DIR}/tools.cpp
    ${DCCORE_DIR}/dbmanager.h ${DCCORE_DIR}/dbmanager.cpp
    ${DCCORE_DIR}/globalsignalemitter.h ${DCCORE_DIR}/globalsignalemitter.cpp

)
if(WIN32)
    list(APPEND SOURCE_LIST ${DCCORE_DIR}/windows/processtracker.cpp
        ${DCCORE_DIR}/windows/wmieventsink.h ${DCCORE_DIR}/windows/wmieventsink.cpp
        ${DCCORE_DIR}/windows/wmiclient.h ${DCCORE_DIR}/windows/wmiclient.cpp)
elseif(APPLE)
     list(APPEND SOURCE_LIST ${DCCORE_DIR}/macos/processtracker.mm ${DCCORE_DIR}/macos/OSXProcessObserver.mm
     ${DCCORE_DIR}/macos/OSXProcessObserver.h)
elseif(UNIX)
    list(APPEND SOURCE_LIST ${DCCORE_DIR}/linux/processtracker.cpp ${DCCORE_DIR}/linux/linuxprocessobserver.cpp
        ${DCCORE_DIR}/linux/linuxprocessobserver.h)
endif()

add_executable(core_tests ${SOURCE_LIST})
add_test(NAME core_tests COMMAND $<TARGET_FILE:core_tests>)

target_include_directories(core_tests PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(core_tests PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(core_tests PUBLIC ${DCAPP_DIR})
target_include_directories(core_tests PUBLIC ${DCCORE_DIR})
target_link_libraries(core_tests PRIVATE qt_libs Qt6::Test DCCore)

install(TARGETS core_tests
    RUNTIME DESTINATION "tests"
    BUNDLE DESTINATION "tests"
    LIBRARY DESTINATION "tests"
)
