set(SRC_MAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/DuneCat.Main)
set(SRC_UI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/DuneCat.UI)
qt_add_executable(DuneCat
    ${SRC_MAIN_DIR}/main.cpp
	
)