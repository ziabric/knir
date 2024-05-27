cmake_minimum_required(VERSION 3.6)

project(knir VERSION 1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories( ${PROJECT_SOURCE_DIR}/ )
include_directories( ${PROJECT_SOURCE_DIR}/imgui )
include_directories( ${PROJECT_SOURCE_DIR}/gui )
include_directories( ${PROJECT_SOURCE_DIR}/algor )
include_directories( ${PROJECT_SOURCE_DIR}/tests )

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "/home/nk/Downloads/SFML-2.6.1/build/")

find_package(SFML COMPONENTS graphics REQUIRED)
find_package(OpenGL REQUIRED)

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
)

add_library(algor STATIC ${ALGOR})

add_executable(test ${TESTS} ${ALGOR})
target_link_libraries(test)

add_executable(${PROJECT_NAME} ${SOURCES} ${GUI} ${IMGUI})
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics OpenGL::GL algor)
