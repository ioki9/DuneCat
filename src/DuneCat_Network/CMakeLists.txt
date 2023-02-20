qt_add_library(DCNetwork STATIC)
qt_add_qml_module(DCNetwork
    URI DCNetwork
    VERSION 1.0
    RESOURCE_PREFIX /DuneCat/qml
    SOURCES
    DCStunClient.cpp  DCStunClient.h
    DCNetworkModels.h
)

target_precompile_headers(DCNetwork
    PUBLIC NetworkHeaders.pch ${CMAKE_SOURCE_DIR}/src/DuneCat_App/EssentialHeaders.pch)
target_include_directories(DCNetwork
    PUBLIC ${CMAKE_SOURCE_DIR}/src/DuneCat_App)
target_link_libraries(DCNetwork PUBLIC qt_libs)
