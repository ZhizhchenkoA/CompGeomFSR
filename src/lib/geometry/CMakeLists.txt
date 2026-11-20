
add_library(geometry STATIC
    convex_polygon_boolean.cpp
    convex_hull.cpp
    delaunay.cpp
    predicates.cpp
    segment_intersection.cpp
)

target_include_directories(geometry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
