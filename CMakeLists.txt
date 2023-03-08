cmake_minimum_required(VERSION 3.16)

project(DuneCat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()


message("Looking for Qt...")
find_package(Qt6 COMPONENTS Quick Qml Network Test REQUIRED)
if (${Qt6_FOUND})
    message("Found Qt " ${Qt6_VERSION})
else()
    message("Couldn't find Qt")
endif()
qt6_standard_project_setup()

set (QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(qt_libs)
target_link_libraries(qt_libs PUBLIC Qt6::Qml Qt6::Quick Qt6::Network Qt6::Gui)
add_dependencies(qt_libs Qt6::Qml Qt6::Quick Qt6::Network Qt6::Gui)
add_subdirectory(src/DuneCat_App)
add_subdirectory(src/DuneCat_Network)
add_subdirectory(tests)

install(TARGETS DuneCat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

configure_file(DuneCatConfig.h.in DuneCatConfig.h)
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
set (
    QML_IMPORT_PATH
    ${CMAKE_BINARY_DIR}/qml
    CACHE
    STRING
    "Path used to locate CMake modules by Qt Creator"
    FORCE
)





