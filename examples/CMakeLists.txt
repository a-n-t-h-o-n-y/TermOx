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

# Scratch ------------------------------------------------------------------------------
add_executable(TermOx.examples.scratch EXCLUDE_FROM_ALL
    scratch.cpp
)
target_compile_options(
    TermOx.examples.scratch
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)
target_link_libraries(
    TermOx.examples.scratch
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
