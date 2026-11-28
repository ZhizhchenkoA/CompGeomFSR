set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

add_executable(GeometryWorkbench
    main.cpp
    mainwindow.h
    mainwindow.cpp
    ui/mainwindow.ui
    geometrypage.h
    geometrypage.cpp
    ui/geometrypage.ui
    geometrycanvas.h
    geometrycanvas.cpp
    longarithmeticpage.h
    longarithmeticpage.cpp
    ui/longarithmeticpage.ui
)

target_include_directories(GeometryWorkbench PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(GeometryWorkbench PRIVATE
    geometry
    bigint
    bezier
    geometry_qt
    Qt${QT_VERSION_MAJOR}::Widgets
)
