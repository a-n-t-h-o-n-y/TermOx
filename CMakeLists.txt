cmake_minimum_required(VERSION 3.18)
project(TermOx VERSION 3.0.0 LANGUAGES CXX)

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
    src/rule.cpp
    src/command.cpp
    src/keyhelp.cpp
    src/theme.cpp
    src/motion.cpp
    src/align.cpp
    src/application.cpp
    src/bordered.cpp
    src/button.cpp
    src/selection.cpp
    src/listview.cpp
    src/select.cpp
    src/menu.cpp
    src/value.cpp
    src/treeview.cpp
    src/logview.cpp
    src/commandpalette.cpp
    src/filepicker.cpp
    src/controls.cpp
    src/checkbox.cpp
    src/datatable.cpp
    src/label.cpp
    src/lineedit.cpp
    src/routing.cpp
    src/focus.cpp
    src/pixelgrid.cpp
    src/put.cpp
    src/radiogroup.cpp
    src/scroll.cpp
    src/scrollbar.cpp
    src/text_view.cpp
    src/timer.cpp
    src/widget.cpp
    src/core/text.cpp
    src/core/frame.cpp
    src/core/layout.cpp
    src/core/canvas.cpp
    src/core/renderer.cpp
    src/core/terminal.cpp
    src/core/event_loop.cpp

    include/ox/rule.hpp
    include/ox/command.hpp
    include/ox/keyhelp.hpp
    include/ox/theme.hpp
    include/ox/motion.hpp
    include/ox/ox.hpp
    include/ox/align.hpp
    include/ox/application.hpp
    include/ox/bordered.hpp
    include/ox/button.hpp
    include/ox/selection.hpp
    include/ox/listview.hpp
    include/ox/select.hpp
    include/ox/menu.hpp
    include/ox/popup.hpp
    include/ox/tabs.hpp
    include/ox/value.hpp
    include/ox/numeric_edit.hpp
    include/ox/value_range.hpp
    include/ox/treeview.hpp
    include/ox/logview.hpp
    include/ox/splitpane.hpp
    include/ox/form.hpp
    include/ox/filepicker.hpp
    include/ox/commandpalette.hpp
    include/ox/controls.hpp
    include/ox/container.hpp
    include/ox/clipboard.hpp
    include/ox/checkbox.hpp
    include/ox/datatable.hpp
    include/ox/routing.hpp
    include/ox/focus.hpp
    include/ox/label.hpp
    include/ox/layout.hpp
    include/ox/lineedit.hpp
    include/ox/detail/text_control.hpp
    include/ox/pixelgrid.hpp
    include/ox/put.hpp
    include/ox/radiogroup.hpp
    include/ox/scroll.hpp
    include/ox/scrollbar.hpp
    include/ox/text_view.hpp
    include/ox/textbox.hpp
    include/ox/timer.hpp
    include/ox/widget.hpp

    include/ox/core/core.hpp
    include/ox/core/events.hpp
    include/ox/core/glyph.hpp
    include/ox/core/geometry.hpp
    include/ox/core/text.hpp
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

find_package(Threads REQUIRED)

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
    GIT_TAG ea7a9dc5bf35b457a8dbe8a16654af1da1668b0d
)
FetchContent_MakeAvailable(signals-light)

# Link External Libraries to the Library Target
target_link_libraries(TermOx PUBLIC
    Threads::Threads
    escape
    signals-light
    zzz
)

# Add the tests subdirectory
enable_testing()
add_subdirectory(tests)
add_subdirectory(examples)
