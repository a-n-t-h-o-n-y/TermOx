cmake_minimum_required(VERSION 3.18)
project(TermOx VERSION 2.0.0 LANGUAGES CXX)

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
    src/focus.cpp
    src/pixel_grid.cpp
    src/put.cpp
    src/scrollbar.cpp
    src/textbox.cpp
    src/widget.cpp
    src/core/terminal.cpp

    include/ox/ox.hpp
    include/ox/align.hpp
    include/ox/application.hpp
    include/ox/bordered.hpp
    include/ox/button.hpp
    include/ox/checkbox.hpp
    include/ox/datatable.hpp
    include/ox/focus.hpp
    include/ox/label.hpp
    include/ox/layout.hpp
    include/ox/lineedit.hpp
    include/ox/pixel_grid.hpp
    include/ox/put.hpp
    include/ox/scrollbar.hpp
    include/ox/textbox.hpp
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
    GIT_TAG 9d7c047f47c81a95a5ea824075253618356593a2
)
FetchContent_MakeAvailable(zzz)

# Escape Library Dependency
FetchContent_Declare(
    escape
    GIT_REPOSITORY https://github.com/a-n-t-h-o-n-y/Escape.git
    GIT_TAG f797e8194b531663f01873f43007d2439fa89276
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
add_subdirectory(tests)
add_subdirectory(examples)