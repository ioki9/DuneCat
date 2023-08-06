qt_add_library(DCCore STATIC)
set(SOURCE_LIST
    processtracker.h processinfo.h
    tools.cpp tools.h
    dbmanager.h dbmanager.cpp
    globalsignalemitter.h globalsignalemitter.cpp)
if(WIN32)
    list(APPEND SOURCE_LIST windows/processtracker.cpp
        windows/wmieventsink.h windows/wmieventsink.cpp
        windows/wmiclient.h windows/wmiclient.cpp)
elseif(APPLE)
    list(APPEND SOURCE_LIST macos/processtracker.mm
        macos/OSXProcessObserver.mm macos/OSXProcessObserver.h)
elseif(UNIX)
    list(APPEND SOURCE_LIST linux/processtracker.cpp linux/linuxprocessobserver.h linux/linuxprocessobserver.cpp)
endif()

qt_add_qml_module(DCCore
    URI DCCore
    VERSION 1.0
    RESOURCE_PREFIX /DuneCat/imports
    SOURCES
    ${SOURCE_LIST}
    )
target_include_directories(DCCore PUBLIC
    ${CMAKE_SOURCE_DIR}/src/DuneCat_App
    ${CMAKE_SOURCE_DIR}/3rdparty
    "${PROJECT_BINARY_DIR}")

target_precompile_headers(DCCore
    PUBLIC ${CMAKE_SOURCE_DIR}/src/DuneCat_App/essentialheaders.h)
target_link_libraries(DCCore PUBLIC qt_libs ${SQLite3})

if(APPLE)
    set_target_properties(DCCore PROPERTIES LINK_FLAGS "-Wl,-F/Library/Frameworks" )
    target_link_libraries(DCCore PUBLIC "-framework Foundation" "-framework Cocoa")
elseif(WIN32)
    target_link_libraries(DCCore PUBLIC wbemuuid version psapi ws2_32)
endif()
