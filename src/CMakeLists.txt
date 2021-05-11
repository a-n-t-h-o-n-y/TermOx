cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
# TermOx Library

if(${CMAKE_VERSION} VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD 17)
endif()

add_library(TermOx STATIC
    common/mb_to_u32.cpp
    common/timer.cpp
    common/u32_to_mb.cpp

    system/detail/filter_send.cpp
    system/detail/send.cpp
    system/detail/is_sendable.cpp
    system/detail/send_shortcut.cpp
    system/detail/event_print.cpp
    system/detail/event_name.cpp
    system/event_queue.cpp
    system/focus.cpp
    system/system.cpp
    system/animation_engine.cpp
    system/user_input_event_loop.cpp
    system/find_widget_at.cpp
    system/event_loop.cpp
    system/shortcuts.cpp

    painter/detail/is_paintable.cpp
    painter/color.cpp
    painter/dynamic_colors.cpp
    painter/painter.cpp
    painter/glyph_matrix.cpp
    painter/glyph_string.cpp

    widget/widgets/detail/nearly_equal.cpp
    widget/widgets/detail/slider_logic.cpp
    widget/widgets/detail/textbox_base.cpp
    widget/widgets/detail/textline_base.cpp
    widget/widgets/detail/textline_core.cpp
    widget/widgets/banner.cpp
    widget/widgets/button.cpp
    widget/widgets/button_list.cpp
    widget/widgets/checkbox.cpp
    widget/widgets/color_select.cpp
    widget/widgets/confirm_button.cpp
    widget/widgets/cycle_box.cpp
    widget/widgets/cycle_stack.cpp
    widget/widgets/label.cpp
    widget/widgets/line.cpp
    widget/widgets/line_edit.cpp
    widget/widgets/log.cpp
    widget/widgets/matrix_view.cpp
    widget/widgets/menu.cpp
    widget/widgets/notify_light.cpp
    widget/widgets/read_file.cpp
    widget/widgets/scrollbar.cpp
    widget/widgets/slider.cpp
    widget/widgets/spinner.cpp
    widget/widgets/text_view.cpp
    widget/widgets/textbox.cpp
    widget/widgets/tile.cpp
    widget/widgets/titlebar.cpp
    widget/widgets/toggle_button.cpp
    widget/widgets/write_file.cpp
    widget/border_offset.cpp
    widget/cursor.cpp
    widget/graph_tree.cpp
    widget/size_policy.cpp
    widget/widget.cpp
    widget/widget_slots.cpp

    terminal/detail/canvas.cpp
    terminal/detail/screen_buffers.cpp
    terminal/terminal.cpp
    terminal/dynamic_color_engine.cpp
)

find_package(Threads REQUIRED)

target_link_libraries(TermOx
    PUBLIC
        signals-light
        escape
    PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(TermOx PUBLIC ${PROJECT_SOURCE_DIR}/include)

include(GNUInstallDirs)
install(TARGETS TermOx
        ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        CONFIGURATIONS Release
)
install(
    DIRECTORY
        ${PROJECT_SOURCE_DIR}/include/termox
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}
)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.8")
    target_compile_features(TermOx PUBLIC cxx_std_17)
endif()

target_compile_options(TermOx
    PRIVATE
        -Wall
        -Wextra
        # -ftime-trace
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
