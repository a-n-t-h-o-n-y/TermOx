cmake_minimum_required(VERSION 3.16)
project(TermOx LANGUAGES CXX)

# Adds runtime checks to the standard library; slow.
# if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#     add_compile_options(-D_GLIBCXX_DEBUG -D_LIBCPP_DEBUG=1)
# endif()

include(${CMAKE_SOURCE_DIR}/cmake/sanitizer.cmake)

set(
    TERMOX_BUILD_TESTS OFF CACHE BOOL
    "Create termox.ui.tests and termox.unit.tests targets"
)

set(
    TERMOX_BUILD_DEMOS OFF CACHE BOOL
    "Create demos and readme.demo targets"
)

# Add Escape Library
add_subdirectory(external/Escape)

# Add Signals-Light
add_subdirectory(external/signals-light)

if (NOT TARGET Catch2)
    add_subdirectory(external/Catch2 EXCLUDE_FROM_ALL)
endif()

# Add TermOx Core
add_subdirectory(src)

# Add Demos
if (TERMOX_BUILD_DEMOS)
    add_subdirectory(demos)
endif()

# Add Tests
if (TERMOX_BUILD_TESTS)
    add_subdirectory(tests)
endif()

add_custom_target(
    termox.all
    DEPENDS
        demos
        readme.demo
        termox.ui.tests
        termox.unit.tests
)
