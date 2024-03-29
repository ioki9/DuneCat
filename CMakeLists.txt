cmake_minimum_required(VERSION 3.16)
if(APPLE)
    project(DuneCat VERSION 0.1 LANGUAGES C CXX OBJCXX OBJC )
    find_library(COCOA_LIBRARY Cocoa)
    if(NOT COCOA_LIBRARY)
        message(FATAL_ERROR "Cocoa not found")
    endif()
else()
    project(DuneCat VERSION 0.1 LANGUAGES CXX)
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
enable_testing()
#find Qt library
message("Looking for Qt...")
find_package(Qt6 COMPONENTS Core Quick Qml Network Test QuickControls2 Sql Widgets REQUIRED)
if (${Qt6_FOUND})
    message("Found Qt " ${Qt6_VERSION})
else()
    message(FATAL_ERROR "Couldn't find Qt")
endif()

qt6_standard_project_setup()
set (QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)


add_library(qt_libs)
target_link_libraries(qt_libs PUBLIC Qt6::Qml Qt6::Quick Qt6::Network Qt6::Gui Qt6::Sql Qt6::Widgets)
add_dependencies(qt_libs Qt6::Qml Qt6::Quick Qt6::Network Qt6::Gui Qt6::Sql Qt6::Widgets)

add_subdirectory(3rdparty/sqlite3)
add_subdirectory(src/DuneCat_App)
add_subdirectory(src/DuneCat_Network)
add_subdirectory(src/DuneCat_Core)
add_subdirectory(tests/core)
#add_subdirectory(tests/networking)


install(TARGETS DuneCat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

configure_file(DuneCatConfig.h.in DuneCatConfig.h)
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty)

set (
    QML_IMPORT_PATH
    ${CMAKE_BINARY_DIR}/qml
    CACHE
    STRING
    "Path used to locate CMake modules by Qt Creator"
    FORCE
)





