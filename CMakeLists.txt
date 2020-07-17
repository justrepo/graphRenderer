cmake_minimum_required(VERSION 3.17)
project(graphRenderer)

set(CMAKE_CXX_STANDARD 17)

add_executable(graphRenderer main.cpp Graph.cpp Graph.h Node.cpp Node.h Link.cpp Link.h TextFactory.cpp TextFactory.h TriangleBoolSquareMatrix.cpp TriangleBoolSquareMatrix.h)

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
if (SFML_FOUND)
    target_link_libraries(${PROJECT_NAME} ${EXECUTABLE_NAME} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
else ()
    message(FATAL_ERROR "Could not find SFML")
endif ()

find_package(TGUI REQUIRED)
if (TGUI_FOUND)
    target_link_libraries(${PROJECT_NAME} tgui)
else ()
    message(FATAL_ERROR "Could not find TGUI")
endif ()