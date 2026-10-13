add_executable(TermOx.tests.unit EXCLUDE_FROM_ALL
    events.test.cpp
    terminal.test.cpp
    widgets.test.cpp
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
