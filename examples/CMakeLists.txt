# SCRATCH ------------------------------------------------------------------------------

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

# PONG ---------------------------------------------------------------------------------

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

# WIDGETS ------------------------------------------------------------------------------

add_executable(TermOx.examples.widgets EXCLUDE_FROM_ALL
    widgets.cpp
)
target_compile_options(
    TermOx.examples.widgets
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)
target_link_libraries(
    TermOx.examples.widgets
    PRIVATE
        TermOx
)

# Pinbox-------------------------------------------------------------------------------\

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