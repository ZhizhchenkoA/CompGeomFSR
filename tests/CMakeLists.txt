if(NOT QT_VERSION_MAJOR EQUAL 6)
    message(FATAL_ERROR "Geometry tests require Qt 6; use -DGEOMETRY_QT_VERSION=6 or -DBUILD_TESTING=OFF")
endif()

find_package(Qt6 REQUIRED COMPONENTS Test)

set(CMAKE_AUTOMOC ON)

foreach(suite IN ITEMS predicates segments hull delaunay polygon_boolean)
    add_executable(test_${suite} test_${suite}.cpp)
    target_link_libraries(test_${suite} PRIVATE geometry Qt6::Test)
    add_test(NAME geometry.${suite} COMMAND test_${suite})
    set_tests_properties(geometry.${suite} PROPERTIES LABELS "geometry;${suite}" TIMEOUT 30)
endforeach()
