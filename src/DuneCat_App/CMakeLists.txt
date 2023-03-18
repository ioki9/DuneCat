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
    NO_RESOURCE_TARGET_PATH
    RESOURCE_PREFIX /DuneCat/imports
    QML_FILES
        "main.qml"
        "qml/controls/DCDeviceTable.qml"
        "qml/controls/DCLaunchButton.qml"
        "qml/pages/DCSettingsPage.qml"
        "qml/pages/DCMainPage.qml"
        "qml/DCStyle.qml"
        "qml/DCMainWindow.qml"
        "qml/DCAdminPanel.qml"
    SOURCES
        "dcsettings.h" "dcsettings.cpp"
    RESOURCES
        "icons/20x20/desktop.png"
        "icons/20x20/circle_indicator.png"
        "icons/20x20/back.png"
        "icons/20x20/drawer.png"
        "icons/20x20/menu.png"
        "icons/20x20@2/desktop.png"
        "icons/20x20@2/back.png"
        "icons/20x20@2/drawer.png"
        "icons/20x20@2/menu.png"
        "icons/20x20@3/back.png"
        "icons/20x20@3/drawer.png"
        "icons/20x20@3/menu.png"
        "icons/20x20@4/back.png"
        "icons/20x20@4/drawer.png"
        "icons/20x20@4/menu.png"
        "icons/index.theme"
        "img/LaunchButton.png"
        "qtquickcontrols2.conf"
)

target_precompile_headers(${PROJECT_NAME} PUBLIC essentialheaders.h)
target_link_libraries(${PROJECT_NAME} PUBLIC qt_libs Qt6::QuickControls2 DCNetwork)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/DuneCat_Network)
