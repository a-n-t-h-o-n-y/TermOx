cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# UI Tests

## Checkbox
add_executable(checkbox.ui.test EXCLUDE_FROM_ALL checkbox.ui.test.cpp)
target_link_libraries(checkbox.ui.test PRIVATE TermOx)

## Spinners
add_executable(spinners.ui.test EXCLUDE_FROM_ALL spinners.ui.test.cpp)
target_link_libraries(spinners.ui.test PRIVATE TermOx)

## Passive Layout Wrapper
add_executable(passive.ui.test EXCLUDE_FROM_ALL passive.ui.test.cpp)
target_link_libraries(passive.ui.test PRIVATE TermOx)

## Fixed Wrapper
add_executable(fixed.ui.test EXCLUDE_FROM_ALL fixed.ui.test.cpp)
target_link_libraries(fixed.ui.test PRIVATE TermOx)

## Layout Sorting
add_executable(layout_sort.ui.test EXCLUDE_FROM_ALL layout_sort.ui.test.cpp)
target_link_libraries(layout_sort.ui.test PRIVATE TermOx)

## Line Edit Widget
add_executable(line_edit.ui.test EXCLUDE_FROM_ALL line_edit.ui.test.cpp)
target_link_libraries(line_edit.ui.test PRIVATE TermOx)

## Slider
add_executable(slider.ui.test EXCLUDE_FROM_ALL slider.ui.test.cpp)
target_link_libraries(slider.ui.test PRIVATE TermOx)

add_custom_target(
    termox.ui.tests
    DEPENDS
        checkbox.ui.test
        spinners.ui.test
        passive.ui.test
        fixed.ui.test
        layout_sort.ui.test
        slider.ui.test
        line_edit.ui.test
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
