function(enable_asan target)
    message(STATUS "Enabling AddressSanitizer for ${target}")

    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/fsanitize=address>
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-fsanitize=address -fno-omit-frame-pointer>
    )

    target_link_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/INCREMENTAL:NO /DEBUG /fsanitize=address>
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-fsanitize=address>
    )
endfunction()

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

enable_asan(TermOx.examples.pong)

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

enable_asan(TermOx.examples.pinbox)

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

enable_asan(TermOx.examples.snake)

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

enable_asan(TermOx.examples.accumulator)

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

# enable_asan(TermOx.examples.scratch)