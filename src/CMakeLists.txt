# CREATE CPPURSES LIBRARY
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
if(${CMAKE_VERSION} VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD 17)
endif()

add_library(cppurses STATIC "")

find_package(Threads REQUIRED)

target_link_libraries(cppurses
    PUBLIC
        signals
        ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE
        nc
)

target_include_directories(cppurses
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

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

# GATHER SOURCES
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
# SYSTEM
target_sources(cppurses PRIVATE
    system/delete_event.cpp
    system/event.cpp
    system/event_loop.cpp
    system/focus.cpp
    system/move_event.cpp
    system/resize_event.cpp
    system/system.cpp
    system/shortcuts.cpp
    system/animation_engine.cpp
    system/timer_event_loop.cpp
    system/user_input_event_loop.cpp
    system/find_widget_at.cpp
    system/mouse.cpp
    system/key.cpp
    system/child_event.cpp
)

# PAINTER
target_sources(cppurses PRIVATE
    painter/painter.cpp
    painter/screen.cpp
    painter/glyph_matrix.cpp
    painter/screen_mask.cpp
    painter/find_empty_space.cpp
    painter/color.cpp
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
    widget/cursor_data.cpp
    widget/graph_tree.cpp
    widget/vertical_slider.cpp
)

# TERMINAL
target_sources(cppurses PRIVATE
    terminal/terminal.cpp
    terminal/output.cpp
    terminal/input.cpp
)

# INSTALLATION
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
install(TARGETS cppurses
    EXPORT CPPursesLibraryConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ../include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT CPPursesLibraryConfig DESTINATION share/CPPursesLibrary/cmake)
export(
    TARGETS cppurses signals
    FILE CPPursesLibraryConfig.cmake
)
