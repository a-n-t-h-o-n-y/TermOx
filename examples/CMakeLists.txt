# SCRATCH ----------------------------------------------------------------------

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

# PONG -------------------------------------------------------------------------

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

# WIDGETS ----------------------------------------------------------------------

add_executable(TermOx.examples.widgets EXCLUDE_FROM_ALL
    widgets_static.cpp
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
# ------------------------------------------------------------------------------