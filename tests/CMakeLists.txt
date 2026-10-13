add_executable(TermOx.tests.unit EXCLUDE_FROM_ALL
    events.test.cpp
    terminal.test.cpp
    widgets.test.cpp
    event_loop.test.cpp
    frame.test.cpp
    renderer.test.cpp
)

target_compile_options(
    TermOx.tests.unit
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_link_libraries(
    TermOx.tests.unit
    PRIVATE
        TermOx
        zzz
)

add_test(NAME TermOx.tests.unit COMMAND TermOx.tests.unit)

# POSIX pseudo-terminal integration is driven by smoke_examples.py.
add_executable(TermOx.tests.renderer_terminal EXCLUDE_FROM_ALL renderer_terminal.cpp)
target_link_libraries(TermOx.tests.renderer_terminal PRIVATE TermOx)
