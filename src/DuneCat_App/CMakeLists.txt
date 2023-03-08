include_directories(${QtQml_INCLUDE_DIRS})
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_add_executable(${PROJECT_NAME}
    main.cpp
    dctools.cpp
    dctools.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE ON
    WIN32_EXECUTABLE TRUE)

qt_add_qml_module(${PROJECT_NAME}
    URI DCBase
    VERSION 1.0
    RESOURCE_PREFIX /DuneCat/qml
    QML_FILES
        main.qml
        dclaunchbutton.qml
    SOURCES
        dcsettings.h dcsettings.cpp
    RESOURCES
        img/LaunchButton.png
)
target_precompile_headers(${PROJECT_NAME} PUBLIC essentialheaders.h)
target_link_libraries(${PROJECT_NAME} PUBLIC qt_libs DCNetwork)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/DuneCat_Network)


