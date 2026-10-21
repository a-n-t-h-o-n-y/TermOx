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

# Browse directories with built-in widgets and asynchronous previews.
add_executable(TermOx.examples.explorer EXCLUDE_FROM_ALL explorer.cpp)
target_compile_options(TermOx.examples.explorer PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(TermOx.examples.explorer PRIVATE TermOx)

# Four interactive simulations with retained scenes and compact controls.
add_executable(TermOx.examples.phase EXCLUDE_FROM_ALL phase.cpp)
target_compile_options(TermOx.examples.phase PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(TermOx.examples.phase PRIVATE TermOx)
