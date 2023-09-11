include_directories(${QtQml_INCLUDE_DIRS})
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_add_executable(${PROJECT_NAME}
    main.cpp
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
        "qml/components/DCSideRoundedRect.qml"
        "qml/components/DCTableHeaderColumn.qml"
        "qml/components/DCTextInputInt.qml"
        "qml/pages/DCSettingsPage.qml"
        "qml/pages/DCMainPage.qml"
        "qml/DCStyle.qml"
        "qml/DCMainWindow.qml"
        "qml/DCAdminPanel.qml"
        "qml/controls/DCProcessTable.qml"
        "qml/controls/DCProcessHistoryTable.qml"
        "qml/controls/DCSegmentedButton.qml"
        "qml/controls/DCSearchTextField.qml"
        "qml/controls/DCDeviceTable.qml"
        "qml/controls/DCToggleButton.qml"
        "qml/controls/DCCloseButton.qml"
        "qml/controls/DCDatePicker.qml"

    SOURCES
        "sortfilterprocessmodel.h" "sortfilterprocessmodel.cpp"
        "processtablemodel.h" "processtablemodel.cpp"
        "settings.h" "settings.cpp" "systemtray.h" "systemtray.cpp"
        "sqltablemodel.h" "sqltablemodel.cpp"
        "sqlsortfiltermodel.h" "sqlsortfiltermodel.cpp"
        "datevalidator.h" "datevalidator.cpp"
        "intvalidator.h" "intvalidator.cpp"
    RESOURCES
        "icons/20x20/desktop.png"
        "icons/20x20/circle_indicator.png"
        "icons/20x20/back.png"
        "icons/20x20/drawer.png"
        "icons/20x20/search.png"
        "icons/20x20/menu.png"
        "icons/20x20/table_view.png"
        "icons/20x20/settings.png"
        "icons/20x20/expand_more.png"
        "icons/20x20/expand_less.png"
        "icons/20x20/refresh.png"
        "icons/20x20/filter_list.png"
        "icons/20x20/close.png"
        "icons/20x20@2/desktop.png"
        "icons/20x20@2/back.png"
        "icons/20x20@2/drawer.png"
        "icons/20x20@2/search.png"
        "icons/20x20@2/menu.png"
        "icons/20x20@2/table_view.png"
        "icons/20x20@2/settings.png"
        "icons/20x20@2/expand_more.png"
        "icons/20x20@2/expand_less.png"
        "icons/20x20@2/refresh.png"
        "icons/20x20@2/filter_list.png"
        "icons/20x20@2/close.png"
        "icons/20x20@3/back.png"
        "icons/20x20@3/drawer.png"
        "icons/20x20@3/menu.png"
        "icons/20x20@4/back.png"
        "icons/20x20@4/drawer.png"
        "icons/20x20@4/menu.png"
        "icons/index.theme"
        "qtquickcontrols2.conf"
)

target_precompile_headers(${PROJECT_NAME} PUBLIC essentialheaders.h)
target_link_libraries(${PROJECT_NAME} PUBLIC qt_libs Qt6::QuickControls2 DCCore DCNetwork )
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC wbemuuid)
endif()
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/src/DuneCat_Core
    ${CMAKE_SOURCE_DIR}/src/DuneCat_Network)
