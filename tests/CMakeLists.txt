add_executable(TermOx.tests.unit EXCLUDE_FROM_ALL
    application.test.cpp
    events.test.cpp
    terminal.test.cpp
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
        Catch2::Catch2WithMain
)

add_executable(scratch EXCLUDE_FROM_ALL
    scratch.cpp
)

target_compile_options(
    scratch
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_link_libraries(
    scratch
    PRIVATE
        TermOx
)