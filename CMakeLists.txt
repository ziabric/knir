cmake_minimum_required(VERSION 3.6)

project(knir VERSION 1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories( ${PROJECT_SOURCE_DIR}/ )
include_directories( ${PROJECT_SOURCE_DIR}/imgui )
include_directories( ${PROJECT_SOURCE_DIR}/gui )
include_directories( ${PROJECT_SOURCE_DIR}/algor )
include_directories( ${PROJECT_SOURCE_DIR}/tests )

find_package(OpenCV REQUIRED)
find_package(SFML COMPONENTS system graphics audio network REQUIRED)

set(IMGUI
        imgui/imconfig.h
        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_internal.h
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        imgui/imconfig-SFML.h
        imgui/imgui-SFML.cpp
        imgui/imgui-SFML.h
        imgui/imgui-SFML_export.h
)

set(SOURCES
        main.cpp
)

file(GLOB GUI
        ${PROJECT_SOURCE_DIR}/gui/*
)

file(GLOB ALGOR
        ${PROJECT_SOURCE_DIR}/algor/*
)

file(GLOB TESTS
        ${PROJECT_SOURCE_DIR}/tests/*
        ${PROJECT_SOURCE_DIR}/algor/*
)

add_library(algor STATIC ${ALGOR})

add_executable(test ${TESTS})
target_link_libraries(test)

if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${GUI} ${IMGUI})
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-window sfml-network sfml-graphics sfml-system opengl32 ${OpenCV_LIBS} algor)
endif()

if(UNIX)
    add_executable(${PROJECT_NAME} ${SOURCES} ${GUI} ${IMGUI})
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-window sfml-network sfml-graphics sfml-system GL ${OpenCV_LIBS} algor)
endif()
