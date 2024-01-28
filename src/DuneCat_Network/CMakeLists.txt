qt_add_library(DCNetwork STATIC)
qt_add_qml_module(DCNetwork
    URI DCNetwork
    VERSION 1.0
    SOURCES
    stunclient.cpp  stunclient.h
    trackermanager.cpp trackermanager.h
    networkmodels.h
)

target_precompile_headers(DCNetwork
    PUBLIC networkheaders.h ${CMAKE_SOURCE_DIR}/src/DuneCat_App/essentialheaders.h)
target_include_directories(DCNetwork
    PUBLIC ${CMAKE_SOURCE_DIR}/src/DuneCat_Core)
target_link_libraries(DCNetwork PUBLIC qt_libs)
