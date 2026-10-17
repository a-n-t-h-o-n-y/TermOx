# Pong ---------------------------------------------------------------------------------
add_executable(TermOx.examples.pong EXCLUDE_FROM_ALL
    pong.cpp
)
target_compile_options(
    TermOx.examples.pong
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)
target_link_libraries(
    TermOx.examples.pong
    PRIVATE
        TermOx
)

# Pinbox -------------------------------------------------------------------------------
add_executable(TermOx.examples.pinbox EXCLUDE_FROM_ALL
    pinbox.cpp
)
target_compile_options(
    TermOx.examples.pinbox
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)
target_link_libraries(
    TermOx.examples.pinbox
    PRIVATE
        TermOx
)

# Snake --------------------------------------------------------------------------------
add_executable(TermOx.examples.snake EXCLUDE_FROM_ALL
    snake.cpp
)
target_compile_options(
    TermOx.examples.snake
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)
target_link_libraries(
    TermOx.examples.snake
    PRIVATE
        TermOx
)

# Accumulator --------------------------------------------------------------------------
add_executable(TermOx.examples.accumulator EXCLUDE_FROM_ALL
    accumulator.cpp
)
target_compile_options(
    TermOx.examples.accumulator
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)
target_link_libraries(
    TermOx.examples.accumulator
    PRIVATE
        TermOx
)

# BMP Viewer ---------------------------------------------------------------------------
add_executable(TermOx.examples.bmp_view EXCLUDE_FROM_ALL
    bmp_view.cpp
)
target_compile_options(
    TermOx.examples.bmp_view
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)
target_link_libraries(
    TermOx.examples.bmp_view
    PRIVATE
        TermOx
)

# Data Table ---------------------------------------------------------------------------
add_executable(TermOx.examples.datatable EXCLUDE_FROM_ALL datatable.cpp)
target_compile_options(TermOx.examples.datatable PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(TermOx.examples.datatable PRIVATE TermOx)

# Selection controls, tabs, menus, and modal dialogs.
add_executable(TermOx.examples.selection EXCLUDE_FROM_ALL selection.cpp)
target_compile_options(TermOx.examples.selection PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(TermOx.examples.selection PRIVATE TermOx)

# Values, tree/log views, split panes, forms, files, and command search.
add_executable(TermOx.examples.views EXCLUDE_FROM_ALL views.cpp)
target_compile_options(TermOx.examples.views PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(TermOx.examples.views PRIVATE TermOx)

# Complete interactive showcase, with optional deterministic headless checks/snapshots.
add_executable(TermOx.examples.showcase EXCLUDE_FROM_ALL showcase.cpp)
target_compile_options(TermOx.examples.showcase PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(TermOx.examples.showcase PRIVATE TermOx)

# Adjustable live stress workloads and headless timings for the library's hot paths.
add_executable(TermOx.examples.stress EXCLUDE_FROM_ALL stress.cpp)
target_compile_options(TermOx.examples.stress PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(TermOx.examples.stress PRIVATE TermOx)

# README counter example.
add_executable(TermOx.examples.counter EXCLUDE_FROM_ALL counter.cpp)
target_link_libraries(TermOx.examples.counter PRIVATE TermOx)
