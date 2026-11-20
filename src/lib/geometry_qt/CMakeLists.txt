find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)

add_library(geometry_qt STATIC
    conversion.cpp
)
target_include_directories(geometry_qt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(geometry_qt PUBLIC
    geometry
    Qt${QT_VERSION_MAJOR}::Gui
)