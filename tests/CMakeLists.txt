find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

set(CMAKE_AUTOMOC ON)

foreach(suite IN ITEMS predicates segments hull delaunay polygon_boolean)
    add_executable(test_${suite} test_${suite}.cpp)
    target_link_libraries(test_${suite} PRIVATE geometry Qt${QT_VERSION_MAJOR}::Test)
    add_test(NAME geometry.${suite} COMMAND test_${suite})
    set_tests_properties(geometry.${suite} PROPERTIES LABELS "geometry;${suite}" TIMEOUT 30)
endforeach()
