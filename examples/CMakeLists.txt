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