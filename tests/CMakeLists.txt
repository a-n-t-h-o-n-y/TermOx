cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# UI Tests

## Checkbox
add_executable(checkbox.ui.test EXCLUDE_FROM_ALL checkbox.ui.test.cpp)
target_link_libraries(checkbox.ui.test PRIVATE TermOx)

add_custom_target(
    termox.ui.tests
    DEPENDS
        checkbox.ui.test
)

# Unit Tests
add_executable(termox.unit.tests EXCLUDE_FROM_ALL
    catch2.main.cpp
    glyph_string.unit.test.cpp
    canvas.unit.test.cpp
    unique_queue.unit.test.cpp
)

# Catch2::Catch2 relies on signals-light defining that target
target_link_libraries(termox.unit.tests PRIVATE TermOx Catch2::Catch2)
