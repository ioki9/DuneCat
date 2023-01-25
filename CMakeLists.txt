cmake_minimum_required(VERSION 3.16)

project(DuneCat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("Looking for Qt...")
find_package(Qt6 COMPONENTS Quick Core REQUIRED)
if (${Qt6_FOUND})
    message("Found Qt " ${Qt6_VERSION})
else()
    message("Couldn't find Qt")
endif()
include("${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt")
qt_add_qml_module(DuneCat
    URI DuneCat
    VERSION 1.0
    QML_FILES src/DuneCat.UI/main.qml
)


set_target_properties(DuneCat PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(DuneCat
    PRIVATE Qt6::Quick Qt6::Core)

install(TARGETS DuneCat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})