cmake_minimum_required(VERSION 3.16)
project(ch10_display_model CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(FLTK REQUIRED)

add_executable(ch10
    main.cpp
    Graph.cpp
    Window.cpp
    GUI.cpp
    Simple_window.cpp
)

target_include_directories(
    ch10 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FLTK_INCLUDE_DIRS}
)
target_link_libraries(ch10 PRIVATE ${FLTK_LIBRARIES})