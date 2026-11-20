cmake_minimum_required(VERSION 3.16)
project(GeometryWorkbench LANGUAGES CXX)

find_package(Doxygen QUIET)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src/lib/geometry)
add_subdirectory(src/lib/bigint)
add_subdirectory(src/lib/bezier)
add_subdirectory(src/lib/geometry_qt)
add_subdirectory(src/apps/GeometryWorkbench)

if(Doxygen_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation")
endif()
