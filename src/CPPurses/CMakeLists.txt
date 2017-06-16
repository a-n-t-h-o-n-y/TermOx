project(libcppurses LANGUAGES CXX)
add_compile_options(-std=c++14)

set(SYSTEM_MODULE_SOURCES
    "src/system_module/thread_data.cpp"
	"src/system_module/system.cpp"
	"src/system_module/event_loop.cpp"
	"src/system_module/ncurses_event_dispatcher.cpp"
	"src/system_module/posted_event_queue.cpp"
	"src/system_module/posted_event.cpp"
	"src/system_module/abstract_event_dispatcher.cpp"
	"src/system_module/object.cpp"
	"src/system_module/move_event.cpp"
	"src/system_module/resize_event.cpp"
	"src/system_module/child_event.cpp"
    )

set(PAINTER_MODULE_SOURCES
    "src/painter_module/ncurses_paint_engine.cpp"
	"src/painter_module/painter.cpp"
	"src/painter_module/paint_engine.cpp"
	"src/painter_module/palette.cpp"
	"src/painter_module/glyph.cpp"
	"src/painter_module/brush.cpp"
	"src/painter_module/geometry.cpp"
	"src/painter_module/paint_buffer.cpp"
    "src/painter_module/glyph_matrix.cpp"
    "src/painter_module/glyph_string.cpp"
    )	

set(WIDGET_MODULE_SOURCES
    "src/widget_module/widget.cpp"
	"src/widget_module/layout.cpp"
	"src/widget_module/horizontal_layout.cpp"
	"src/widget_module/border.cpp"
    "src/widget_module/vertical_layout.cpp"
    "src/widget_module/textbox.cpp"
    "src/widget_module/textbox_base.cpp"
    "src/widget_module/push_button.cpp"
    "src/widget_module/horizontal_scrollbar.cpp"
    "src/widget_module/vertical_scrollbar.cpp"
    "src/widget_module/checkbox.cpp"
    "src/widget_module/titlebar.cpp"
    "src/widget_module/text_display.cpp"
    "src/widget_module/color_chooser.cpp"
    )

set(TEST_SOURCES 
    "test/system_module/system_test.cpp"
	"test/system_module/object_test.cpp"
	"test/system_module/event_loop_test.cpp"
	"test/system_module/event_test.cpp"
	"test/system_module/abstract_event_dispatcher_test.cpp"
	"test/system_module/thread_data_test.cpp"
	"test/system_module/posted_event_queue_test.cpp"
	"test/system_module/posted_event_test.cpp"
	"test/system_module/ncurses_event_dispatcher_test.cpp"

	"test/widget_module/widget_test.cpp"

	"test/painter_module/glyph_test.cpp"
	"test/painter_module/glyph_string_test.cpp"
	"test/painter_module/brush_test.cpp"
	"test/painter_module/palette_test.cpp"
	"test/painter_module/glyph_matrix_test.cpp"
    )

add_library(cppurses STATIC
    ${SYSTEM_MODULE_SOURCES}
	${PAINTER_MODULE_SOURCES}
    ${WIDGET_MODULE_SOURCES}
    )

target_include_directories(cppurses PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/cppurses>
    $<INSTALL_INTERFACE:include/cppurses>
    PRIVATE src)

install(TARGETS cppurses EXPORT CPPursesConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# For find_package
install(EXPORT CPPursesConfig DESTINATION share/CPPurses/cmake)
export(TARGETS cppurses FILE CPPursesConfig.cmake)

# Demos
add_executable(notepad demos/notepad_demo.cpp)
target_link_libraries(notepad cppurses ncurses)

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
