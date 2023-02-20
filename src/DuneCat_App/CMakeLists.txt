include_directories(${QtQml_INCLUDE_DIRS})
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_add_executable(${PROJECT_NAME}
    main.cpp
    DCTools.cpp
    DCTools.h
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
        DCLaunchButton.qml
    SOURCES
        DCSettings.h DCSettings.cpp
    RESOURCES
        img/LaunchButton.png
)
target_precompile_headers(${PROJECT_NAME} PUBLIC EssentialHeaders.pch)
target_link_libraries(${PROJECT_NAME} PUBLIC qt_libs DCNetwork)
