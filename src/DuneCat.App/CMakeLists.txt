
qt_add_executable(${PROJECT_NAME}
    main.cpp
    tools.h
    tools.cpp
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Qml Qt6::Quick)

    set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE)

qt_add_qml_module(${PROJECT_NAME}
URI UI_Basics
VERSION 1.0
QML_FILES UI/main.qml
)

install(TARGETS DuneCat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
