add_executable(termox.tests.unit EXCLUDE_FROM_ALL
    test.test.cpp
)

target_compile_options(
    termox.tests.unit
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_link_libraries(
    termox.tests.unit
    PRIVATE
        TermOx
        Catch2::Catch2WithMain
)