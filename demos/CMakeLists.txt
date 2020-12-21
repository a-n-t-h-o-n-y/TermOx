if(${CMAKE_VERSION} VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD 17)
endif()

add_executable(demos EXCLUDE_FROM_ALL "")
target_compile_options(demos PRIVATE -Wall -Wextra -Wpedantic)

# GLYPH PAINT
target_sources(demos PRIVATE
    glyph_paint/glyph_paint.cpp
    glyph_paint/paint_area.cpp
    glyph_paint/options_box.cpp
    glyph_paint/glyph_selector.cpp
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

target_link_libraries(demos PUBLIC cppurses)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.8")
    target_compile_features(demos INTERFACE cxx_std_17)
endif()
