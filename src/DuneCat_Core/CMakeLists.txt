qt_add_library(DCCore STATIC)
set(SOURCE_LIST
    dcprocesstracker.h dcprocessinfo.h
)
if(WIN32)
    list(APPEND SOURCE_LIST windows/dcprocesstracker.cpp
        windows/wmieventsink.h windows/wmieventsink.cpp
        windows/wmiclient.h windows/wmiclient.cpp)
elseif(APPLE)
    list(APPEND SOURCE_LIST macos/dcprocesstracker.cpp)
endif()
qt_add_qml_module(DCCore
    URI DCCore
    VERSION 1.0
    RESOURCE_PREFIX /DuneCat/imports
    SOURCES
    ${SOURCE_LIST}
    )
target_precompile_headers(DCCore
    PUBLIC ${CMAKE_SOURCE_DIR}/src/DuneCat_App/essentialheaders.h)
target_include_directories(DCCore
    PUBLIC ${CMAKE_SOURCE_DIR}/src/DuneCat_App)
target_link_libraries(DCCore PUBLIC qt_libs)
