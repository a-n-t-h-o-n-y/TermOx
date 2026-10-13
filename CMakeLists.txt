cmake_minimum_required(VERSION 3.18)
project(TermOx VERSION 2.0.0 LANGUAGES CXX)

option(USE_ASAN "Enable AddressSanitizer" OFF)

if (TERMOX_USE_ASAN)
    message(STATUS "Enabling AddressSanitizer for all targets.")

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/fsanitize=address /Zi)
        add_link_options(/fsanitize=address /INCREMENTAL:NO)
    endif()
endif()


# Create Library Target
add_library(TermOx STATIC
    src/align.cpp
    src/application.cpp
    src/bordered.cpp
    src/button.cpp
    src/checkbox.cpp
    src/datatable.cpp
    src/label.cpp
    src/lineedit.cpp
    src/routing.cpp
    src/focus.cpp
    src/pixelgrid.cpp
    src/put.cpp
    src/radiogroup.cpp
    src/scrollbar.cpp
    src/textbox.cpp
    src/timer.cpp
    src/widget.cpp
    src/core/frame.cpp
    src/core/layout.cpp
    src/core/canvas.cpp
    src/core/renderer.cpp
    src/core/terminal.cpp
    src/core/event_loop.cpp

    include/ox/ox.hpp
    include/ox/align.hpp
    include/ox/application.hpp
    include/ox/bordered.hpp
    include/ox/button.hpp
    include/ox/checkbox.hpp
    include/ox/datatable.hpp
    include/ox/routing.hpp
    include/ox/focus.hpp
    include/ox/label.hpp
    include/ox/layout.hpp
    include/ox/lineedit.hpp
    include/ox/pixelgrid.hpp
    include/ox/put.hpp
    include/ox/radiogroup.hpp
    include/ox/scrollbar.hpp
    include/ox/textbox.hpp
    include/ox/timer.hpp
    include/ox/widget.hpp

    include/ox/core/common.hpp
    include/ox/core/core.hpp
    include/ox/core/events.hpp
    include/ox/core/glyph.hpp
    include/ox/core/geometry.hpp
    include/ox/core/frame.hpp
    include/ox/core/layout.hpp
    include/ox/core/canvas.hpp
    include/ox/core/renderer.hpp
    include/ox/core/dispatch.hpp
    include/ox/core/terminal.hpp
    include/ox/core/event_loop.hpp
)

# Include Directories for the Library
target_include_directories(TermOx PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Compiler Options for the Library
target_compile_options(TermOx
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra -Wpedantic -Wconversion>
)

include(FetchContent)

# zzz Library Dependency
FetchContent_Declare(
    zzz
    GIT_REPOSITORY https://github.com/a-n-t-h-o-n-y/zzz.git
    GIT_TAG 8989d54a07863de079915406084cdf332675ed7e
)
FetchContent_MakeAvailable(zzz)

# Escape Library Dependency
FetchContent_Declare(
    escape
    GIT_REPOSITORY https://github.com/a-n-t-h-o-n-y/Escape.git
    GIT_TAG c714abfa0061526ec0d9ab593b4a8ab447c1d412
)
FetchContent_MakeAvailable(escape)

# SignalsLight Library Dependency
FetchContent_Declare(
    signals-light
    GIT_REPOSITORY https://github.com/a-n-t-h-o-n-y/signals-light.git
    GIT_TAG 5b873d0aa515a2085138359e69746cd7d7a5362f
)
FetchContent_MakeAvailable(signals-light)

# Link External Libraries to the Library Target
target_link_libraries(TermOx PUBLIC
    escape
    signals-light
    zzz
)

# Add the tests subdirectory
enable_testing()
add_subdirectory(tests)
add_subdirectory(examples)
