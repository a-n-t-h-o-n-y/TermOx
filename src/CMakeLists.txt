# FIND LOCAL NCURSES
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/")
set(CURSES_NEED_NCURSE TRUE)
find_package(Ncursesw REQUIRED)
add_library(NCurses INTERFACE IMPORTED)
message(STATUS CURSES_INCLUDE_DIR:${CURSES_INCLUDE_DIR})
if(CURSES_HAVE_NCURSESW_NCURSES_H AND NCURSESW_FOUND)
    if(${OSX_BREW_NCURSES})
      set(NCURSES_INCLUDE_PATH ${CURSES_INCLUDE_DIR})
    else()
      set(NCURSES_INCLUDE_PATH ${CURSES_INCLUDE_DIR}/ncursesw)
    endif()
elseif(CURSES_HAVE_NCURSES_NCURSES_H)
    set(NCURSES_INCLUDE_PATH ${CURSES_INCLUDE_DIR}/ncurses)
elseif(CURSES_HAVE_NCURSES_H)
    set(NCURSES_INCLUDE_PATH ${CURSES_INCLUDE_DIR})
endif()
message(STATUS NCURSES_INCLUDE_PATH:${NCURSES_INCLUDE_PATH})
message(STATUS CURSES_LIBRARIES:${CURSES_LIBRARIES})
set_property(TARGET NCurses
    PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${NCURSES_INCLUDE_PATH}
)
set_property(TARGET NCurses
    PROPERTY
        INTERFACE_LINK_LIBRARIES ${CURSES_LIBRARIES}
)

# CREATE CPPURSES LIBRARY
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
if(${CMAKE_VERSION} VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD 14)
endif()
add_library(cppurses STATIC "")
target_link_libraries(cppurses PUBLIC signals optional PRIVATE NCurses)
target_include_directories(cppurses
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
if(NOT ${CMAKE_VERSION} VERSION_LESS "3.8")
    target_compile_features(cppurses INTERFACE cxx_std_14)
endif()

target_compile_options(cppurses PRIVATE -Wall)

# GATHER SOURCES
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
# SYSTEM
target_sources(cppurses PRIVATE
    system/delete_event.cpp
    system/event.cpp
    system/event_invoker.cpp
    system/event_loop.cpp
    system/event_queue.cpp
    system/focus.cpp
    system/key.cpp
    system/key_event.cpp
    system/move_event.cpp
    system/resize_event.cpp
    system/system.cpp
    system/shortcuts.cpp
    system/animation_engine.cpp
    system/event_as_string.cpp
    system/timer_event_loop.cpp
    system/user_input_event_loop.cpp
    system/fps_to_period.cpp
    system/find_widget_at.cpp
    system/terminal_resize_event.cpp
)

# PAINTER
target_sources(cppurses PRIVATE
    painter/painter.cpp
    painter/brush.cpp
    painter/screen.cpp
    painter/glyph_matrix.cpp
    painter/glyph_string.cpp
    painter/wchar_to_bytes.cpp
    painter/extended_char.cpp
    painter/screen_mask.cpp
    painter/find_empty_space.cpp
    painter/screen_state.cpp
    painter/palettes.cpp
    painter/color.cpp
)	

# WIDGET
target_sources(cppurses PRIVATE
    widget/widget.cpp
    widget/widget.event_handlers.cpp
    widget/widget_slots.cpp
    widget/widget_stack.cpp
    widget/widget_stack_menu.cpp
    widget/glyph_select_stack.cpp
    widget/glyph_select.cpp
    widget/cycle_stack.cpp
    widget/label.cpp
    widget/layout.cpp
    widget/line_edit.cpp
    widget/log.cpp
    widget/confirm_button.cpp
    widget/labeled_cycle_box.cpp
    widget/horizontal_layout.cpp
    widget/matrix_display.cpp
    widget/point.cpp
    widget/border.cpp
    widget/cycle_box.cpp
    widget/vertical_layout.cpp
    widget/status_bar.cpp
    widget/textbox.cpp
    widget/textbox_slots.cpp
    widget/horizontal_slider.cpp
    widget/textbox_base.cpp
    widget/push_button.cpp
    widget/horizontal_scrollbar.cpp
    widget/vertical_scrollbar.cpp
    widget/checkbox.cpp
    widget/titlebar.cpp
    widget/text_display.cpp
    widget/color_select.cpp
    widget/menu.cpp
    widget/size_policy.cpp
    widget/blank_width.cpp
    widget/blank_height.cpp
    widget/children_data.cpp
    widget/border_offset.cpp
    widget/cursor_data.cpp
    widget/widget_free_functions.cpp
    widget/graph_tree.cpp
    widget/nearly_equal.cpp
    widget/vertical_slider.cpp
    widget/slider_logic.cpp
    widget/toggle_button.cpp
)

# TERMINAL
target_sources(cppurses PRIVATE
    terminal/terminal.cpp
    terminal/output.cpp
    terminal/input.cpp
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
    TARGETS cppurses signals optional
    FILE CPPursesLibraryConfig.cmake
)

# CLANG TIDY
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(compile_commands ALL
	COMMAND [ -f ${CMAKE_SOURCE_DIR}/build/compile_commands.json ] && mv
        ${CMAKE_SOURCE_DIR}/build/compile_commands.json
		${CMAKE_SOURCE_DIR}/compile_commands.json || :)

