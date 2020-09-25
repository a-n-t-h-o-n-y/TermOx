if(${CMAKE_VERSION} VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD 17)
endif()

add_executable(demos EXCLUDE_FROM_ALL "")
target_compile_options(demos PRIVATE -Wall -Wextra -Wpedantic)

# CHESS
# TODO Include subdirectory instead.
target_sources(demos PRIVATE
    chess/src/chessboard_widget.cpp
    chess/src/position.cpp
    chess/src/chess_engine.cpp
    chess/src/rules.cpp
    chess/src/state.cpp
    chess/src/chess_ui.cpp
    chess/src/side_pane.cpp
    chess/src/settings_pane.cpp
    chess/src/lower_pane.cpp
    chess/src/move_input.cpp
    chess/src/piece.cpp
    chess/src/move.cpp
    chess/src/standard_rules.cpp
    chess/src/no_rules.cpp
    chess/src/shared_user_input.cpp
    chess/src/player_human.cpp
    chess/src/chess_event_loop.cpp
    chess/src/shared_move.cpp
)

# GLYPH PAINT
target_sources(demos PRIVATE
    glyph_paint/glyph_paint.cpp
    glyph_paint/paint_area.cpp
    glyph_paint/options_box.cpp
    glyph_paint/populated_glyph_stack.cpp
    glyph_paint/glyph_select_stack.cpp
    glyph_paint/glyph_select.cpp
)

# PALETTE
target_sources(demos PRIVATE
    palette/value_control.cpp
    palette/color_definition_setter.cpp
    palette/color_control.cpp
    palette/palette_helpers.cpp
    palette/export_panel.cpp
    palette/all_colors_display.cpp
)

# NOTEPAD / MAIN MENU
target_sources(demos PRIVATE
    demo.main.cpp
    notepad/notepad.cpp
)

# GAME OF LIFE
target_sources(demos PRIVATE
    game_of_life/gol_widget.cpp
    game_of_life/patterns.cpp
    game_of_life/gol_demo.cpp
    game_of_life/exporters.cpp
    game_of_life/filetype.cpp
    game_of_life/get_rle.cpp
    game_of_life/get_life_1_05.cpp
    game_of_life/get_life_1_06.cpp
    game_of_life/get_plaintext.cpp
)

target_link_libraries(demos PRIVATE cppurses)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.8")
    target_compile_features(demos INTERFACE cxx_std_17)
endif()
