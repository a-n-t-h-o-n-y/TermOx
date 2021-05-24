cmake_minimum_required(VERSION 3.9)

if(${CMAKE_VERSION} VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD 17)
endif()

add_executable(demos EXCLUDE_FROM_ALL "")

target_compile_options(demos
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

# Glyph Paint
target_sources(demos
    PRIVATE
        glyph_paint/glyph_paint.cpp
        glyph_paint/paint_area.cpp
        glyph_paint/options_box.cpp
        glyph_paint/glyph_selector.cpp
)

# Notepad / Main Menu
target_sources(demos
    PRIVATE
        demo.main.cpp
        notepad/notepad.cpp
        snake/snake.cpp
        graph/graph_demo.cpp
)

# Conway's Game of Life
target_sources(demos
    PRIVATE
        game_of_life/gol_widget.cpp
        game_of_life/patterns.cpp
        game_of_life/gol_demo.cpp
        game_of_life/exporters.cpp
        game_of_life/filetype.cpp
        game_of_life/get_rle.cpp
        game_of_life/get_life_1_05.cpp
        game_of_life/get_life_1_06.cpp
        game_of_life/get_plaintext.cpp
        game_of_life/bitmap.cpp
        game_of_life/bitset.cpp
)

target_link_libraries(demos PUBLIC TermOx)

add_executable(readme.demo EXCLUDE_FROM_ALL readme.main.cpp)
target_link_libraries(readme.demo PUBLIC TermOx)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.8")
    target_compile_features(demos INTERFACE cxx_std_17)
endif()
