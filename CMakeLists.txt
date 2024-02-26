cmake_minimum_required(VERSION 3.18)
project(TermOx VERSION 2.0.0 LANGUAGES CXX)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add External Dependencies
add_subdirectory(external/Escape)
add_subdirectory(external/signals-light)

# Create Library Target
add_library(TermOx STATIC
    src/terminal.cpp
    src/widgets/focus.cpp

    include/termox/common.hpp
    include/termox/core.hpp
    include/termox/events.hpp
    include/termox/glyph.hpp
    include/termox/terminal.hpp

    include/termox/widgets/application.hpp
    include/termox/widgets/focus.hpp
    include/termox/widgets/layout.hpp
    include/termox/widgets/widget.hpp
)

# Include Directories for the Library
target_include_directories(TermOx PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Compiler Options for the Library
target_compile_options(TermOx PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wpedantic -Wconversion>
)

# Link External Libraries to the Library Target
target_link_libraries(TermOx PUBLIC
    escape
    signals-light
)

# Add the tests subdirectory
add_subdirectory(tests)
add_subdirectory(examples)