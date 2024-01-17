add_executable(demos EXCLUDE_FROM_ALL "")

# Glyph Paint
target_sources(demos
    PRIVATE
        glyph_paint/glyph_paint.cpp
        glyph_paint/paint_area.cpp
        glyph_paint/options_box.cpp
        glyph_paint/glyph_selector.cpp
)

# Everything Else / Main Menu
target_sources(demos
    PRIVATE
        demo.main.cpp
        notepad/notepad.cpp
        snake/snake.cpp
        graph/graph_demo.cpp
        fractal/fractal_demo.cpp
)

# Conway's Game of Life
target_sources(demos
    PRIVATE
        game_of_life/game_of_life_engine.cpp
        game_of_life/gol_widget.cpp
        game_of_life/patterns.cpp
        game_of_life/gol_demo.cpp
        game_of_life/exporters.cpp
        game_of_life/filetype.cpp
        game_of_life/get_rle.cpp
        game_of_life/get_life_1_05.cpp
        game_of_life/get_life_1_06.cpp
        game_of_life/get_plaintext.cpp
        game_of_life/bitset.cpp
)

target_link_libraries(demos
    PUBLIC
        TermOx
)

target_compile_options(demos
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

add_executable(readme.demo EXCLUDE_FROM_ALL readme.main.cpp)
target_link_libraries(readme.demo PUBLIC TermOx)
target_compile_options(readme.demo PRIVATE -Wall -Wextra -Wpedantic)
