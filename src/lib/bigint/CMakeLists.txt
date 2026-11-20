add_library(bigint STATIC
    bigint.cpp
    exact_predicate.cpp
)

target_include_directories(bigint PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
