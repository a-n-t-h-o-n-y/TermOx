add_executable(TermOx.tests.unit EXCLUDE_FROM_ALL
    events.test.cpp
    terminal.test.cpp
    widgets.test.cpp
    widget_api.test.cpp
    event_loop.test.cpp
    frame.test.cpp
    renderer.test.cpp
    layout.test.cpp
    layout_widgets.test.cpp
    routing.test.cpp
    text.test.cpp
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

# Headless benchmarks; build explicitly in Release mode and run directly.
add_executable(TermOx.tests.benchmarks EXCLUDE_FROM_ALL benchmarks.cpp)
target_link_libraries(TermOx.tests.benchmarks PRIVATE TermOx)

# Isolate allocation fault injection from the regular tests and their worker threads.
add_executable(TermOx.tests.text_allocation EXCLUDE_FROM_ALL text_allocation.test.cpp)
target_link_libraries(TermOx.tests.text_allocation PRIVATE TermOx zzz)
add_dependencies(TermOx.tests.unit TermOx.tests.text_allocation)
add_test(NAME TermOx.tests.text_allocation COMMAND TermOx.tests.text_allocation)

# PTY probe for idle waiting and cross-thread delivery, without synthetic input.
add_executable(TermOx.tests.event_loop_terminal EXCLUDE_FROM_ALL event_loop_terminal.cpp)
target_link_libraries(TermOx.tests.event_loop_terminal PRIVATE TermOx)
