# CREATE CPPURSES LIBRARY
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
if(${CMAKE_VERSION} VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD 17)
endif()

add_library(cppurses STATIC "")

find_package(Threads REQUIRED)

target_link_libraries(cppurses
    PUBLIC
        signals-light
        ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE
        nc
)

target_include_directories(cppurses PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.8")
    target_compile_features(cppurses PUBLIC cxx_std_17)
endif()

target_compile_options(cppurses
    PRIVATE
        -Wall
        -Wextra
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
target_compile_options(cppurses
    PUBLIC
        -Wno-maybe-uninitialized
        -Wno-unused-but-set-variable
)
endif()

# Enable Someday if needed
# if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16")
#     target_precompile_headers(
#         cppurses
#         PRIVATE
#             ../include/cppurses/widget/widget.hpp
#             ../external/signals-light/include/signals_light/signal.hpp
#     )
# endif()

# GATHER SOURCES
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
# SYSTEM
target_sources(cppurses PRIVATE
    system/event_loop.cpp
    system/focus.cpp
    system/system.cpp
    system/animation_engine.cpp
    system/timer_event_loop.cpp
    system/user_input_event_loop.cpp
    system/find_widget_at.cpp
)

# PAINTER
target_sources(cppurses PRIVATE
    painter/painter.cpp
    painter/screen.cpp
    painter/glyph_matrix.cpp
    painter/screen_mask.cpp
    painter/find_empty_space.cpp
)

# WIDGET
target_sources(cppurses PRIVATE
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

# TERMINAL
target_sources(cppurses PRIVATE
    terminal/terminal.cpp
    terminal/output.cpp
    terminal/input.cpp
    terminal/dynamic_color_engine.cpp
)
