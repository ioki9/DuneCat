qt_add_qml_module(DCNetwork
    URI DCNetwork
    VERSION 1.0
    RESOURCE_PREFIX /DuneCat/qml
    SOURCES
    DCStunClient.cpp  DCStunClient.h
    DCNetworkModels.h
)
target_precompile_headers(DCNetwork PUBLIC NetworkHeaders.pch)
