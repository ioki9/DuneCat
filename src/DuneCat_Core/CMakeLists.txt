qt_add_library(DCCore STATIC)
qt_add_qml_module(DCCore
    URI DCCore
    VERSION 1.0
    RESOURCE_PREFIX /DuneCat/imports
    SOURCES
    wmieventsink.h wmieventsink.cpp
    wmiclient.h wmiclient.cpp
    dcprocesstracker.cpp dcprocesstracker.h dcprocessinfo.h
)
target_precompile_headers(DCCore
    PUBLIC ${CMAKE_SOURCE_DIR}/src/DuneCat_App/essentialheaders.h)
target_include_directories(DCCore
    PUBLIC ${CMAKE_SOURCE_DIR}/src/DuneCat_App)
target_link_libraries(DCCore PUBLIC qt_libs)
