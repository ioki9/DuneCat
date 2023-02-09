qt_add_executable(${PROJECT_NAME}
    main.cpp
    tools.h
    tools.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Qml Qt6::Quick)
    
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE ON
    WIN32_EXECUTABLE TRUE)

qt_add_qml_module(${PROJECT_NAME}
    URI UI
    VERSION 1.0
    QML_FILES main.qml
    )
