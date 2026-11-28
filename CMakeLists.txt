cmake_minimum_required(VERSION 3.16)
project(GeometryWorkbench LANGUAGES CXX)

include(CTest)

find_package(Doxygen QUIET)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GEOMETRY_QT_VERSION "" CACHE STRING "Qt major version (5 or 6; empty for automatic selection)")
if(GEOMETRY_QT_VERSION)
    if(NOT GEOMETRY_QT_VERSION MATCHES "^[56]$")
        message(FATAL_ERROR "GEOMETRY_QT_VERSION must be 5 or 6")
    endif()
    find_package(Qt${GEOMETRY_QT_VERSION} REQUIRED COMPONENTS Widgets)
    set(QT_VERSION_MAJOR ${GEOMETRY_QT_VERSION})
else()
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
endif()

add_subdirectory(src/lib/geometry)
add_subdirectory(src/lib/bigint)
add_subdirectory(src/lib/bezier)
add_subdirectory(src/lib/geometry_qt)
add_subdirectory(src/apps/GeometryWorkbench)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(Doxygen_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation")
endif()
