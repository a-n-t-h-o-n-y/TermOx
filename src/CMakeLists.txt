cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
# TermOx Library

if(${CMAKE_VERSION} VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD 17)
endif()

add_library(TermOx STATIC "")

find_package(Threads REQUIRED)

target_link_libraries(TermOx
    PUBLIC
        signals-light
        ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE
        nc
)

target_include_directories(TermOx PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.8")
    target_compile_features(TermOx PUBLIC cxx_std_17)
endif()

target_compile_options(TermOx
    PRIVATE
        -Wall
        -Wextra
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
target_compile_options(TermOx
    PUBLIC
        -Wno-maybe-uninitialized
        -Wno-unused-but-set-variable
)
endif()

# Enable Someday if needed
# if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16")
#     target_precompile_headers(
#         TermOx
#         PRIVATE
#             ../include/termox/widget/widget.hpp
#             ../external/signals-light/include/signals_light/signal.hpp
#     )
# endif()

# Gather Sources
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

# System
target_sources(TermOx
    PRIVATE
        system/event_loop.cpp
        system/focus.cpp
        system/system.cpp
        system/animation_engine.cpp
        system/timer_event_loop.cpp
        system/user_input_event_loop.cpp
        system/find_widget_at.cpp
)

# Painter
target_sources(TermOx
    PRIVATE
        painter/painter.cpp
        painter/screen.cpp
        painter/glyph_matrix.cpp
        painter/screen_mask.cpp
        painter/find_empty_space.cpp
)

# Widget
target_sources(TermOx
    PRIVATE
        widget/widget.cpp
        widget/widget_slots.cpp
        widget/line_edit.cpp
        widget/log.cpp
        widget/border.cpp
        widget/cycle_box.cpp
        widget/textbox.cpp
        widget/horizontal_slider.cpp
        widget/textbox_base.cpp
        widget/text_display.cpp
        widget/menu.cpp
        widget/border_offset.cpp
        widget/graph_tree.cpp
        widget/vertical_slider.cpp
)

# Terminal
target_sources(TermOx
    PRIVATE
        terminal/terminal.cpp
        terminal/output.cpp
        terminal/input.cpp
        terminal/dynamic_color_engine.cpp
)
