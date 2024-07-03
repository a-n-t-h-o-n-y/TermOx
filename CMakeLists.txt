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
    src/application.cpp
    # src/border.cpp
    src/label.cpp
    src/focus.cpp
    src/layout.cpp
    # src/text_box.cpp
    src/widget.cpp
    src/core/terminal.cpp

    include/ox/ox.hpp
    include/ox/application.hpp
    # include/ox/border.hpp
    include/ox/button.hpp
    include/ox/focus.hpp
    include/ox/label.hpp
    include/ox/layout.hpp
    # include/ox/text_box.hpp
    include/ox/widget.hpp

    include/ox/core/common.hpp
    include/ox/core/core.hpp
    include/ox/core/events.hpp
    include/ox/core/glyph.hpp
    include/ox/core/terminal.hpp
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