cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(libcppurses LANGUAGES CXX)

# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(libs/Signals)
include(GNUInstallDirs)
enable_testing()
add_compile_options(-std=c++14)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cppurses
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/Signals/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/Signals/libs/Optional/include
    )

set(SYSTEM_SOURCES
    "src/system/thread_data.cpp"
	"src/system/system.cpp"
	"src/system/event_loop.cpp"
	"src/system/ncurses_event_dispatcher.cpp"
	"src/system/posted_event_queue.cpp"
	"src/system/posted_event.cpp"
	"src/system/abstract_event_dispatcher.cpp"
	"src/system/object.cpp"
	"src/system/move_event.cpp"
	"src/system/resize_event.cpp"
	"src/system/child_event.cpp"
    )

set(PAINTER_SOURCES
    "src/painter/ncurses_paint_engine.cpp"
	"src/painter/painter.cpp"
	"src/painter/paint_engine.cpp"
	"src/painter/palette.cpp"
	"src/painter/glyph.cpp"
	"src/painter/brush.cpp"
	"src/painter/geometry.cpp"
	"src/painter/paint_buffer.cpp"
    "src/painter/glyph_matrix.cpp"
    "src/painter/glyph_string.cpp"
    )	

set(WIDGET_SOURCES
    "src/widget/widget.cpp"
	"src/widget/layout.cpp"
	"src/widget/horizontal_layout.cpp"
	"src/widget/border.cpp"
    "src/widget/vertical_layout.cpp"
    "src/widget/textbox.cpp"
    "src/widget/textbox_base.cpp"
    "src/widget/push_button.cpp"
    "src/widget/horizontal_scrollbar.cpp"
    "src/widget/vertical_scrollbar.cpp"
    "src/widget/checkbox.cpp"
    "src/widget/titlebar.cpp"
    "src/widget/text_display.cpp"
    "src/widget/color_chooser.cpp"
    "src/widget/menu.cpp"
    )

set(TEST_SOURCES 
    "test/system/system_test.cpp"
	"test/system/object_test.cpp"
	"test/system/event_loop_test.cpp"
	"test/system/event_test.cpp"
	"test/system/abstract_event_dispatcher_test.cpp"
	"test/system/thread_data_test.cpp"
	"test/system/posted_event_queue_test.cpp"
	"test/system/posted_event_test.cpp"
	"test/system/ncurses_event_dispatcher_test.cpp"

	"test/widget/widget_test.cpp"

	"test/painter/glyph_test.cpp"
	"test/painter/glyph_string_test.cpp"
	"test/painter/brush_test.cpp"
	"test/painter/palette_test.cpp"
	"test/painter/glyph_matrix_test.cpp"
    )

add_library(cppurses STATIC
    ${SYSTEM_SOURCES}
	${PAINTER_SOURCES}
    ${WIDGET_SOURCES}
    )

install(TARGETS cppurses
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Demos
add_executable(notepad demos/notepad_demo.cpp)
target_link_libraries(notepad cppurses ncurses)
add_executable(menu demos/menu_demo.cpp)
target_link_libraries(menu cppurses ncurses)

# Tests
add_executable(testcppurses ${TEST_SOURCES})
target_link_libraries(testcppurses cppurses ncurses)

find_package(GTest REQUIRED)
if (GTEST_FOUND)
    target_link_libraries(testcppurses ${GTEST_BOTH_LIBRARIES})
endif()

add_test(testcppurses testcppurses)

# Build System Info For Clang-Tidy
if (NOT TARGET compile_commands)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_custom_target(compile_commands ALL
    	COMMAND [ -f ${CMAKE_SOURCE_DIR}/build/compile_commands.json ] && mv
            ${CMAKE_SOURCE_DIR}/build/compile_commands.json
    		${CMAKE_SOURCE_DIR}/compile_commands.json || :)
endif()

# Doxygen Target
find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(docs
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()
