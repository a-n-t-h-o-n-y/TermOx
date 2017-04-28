cmake_minimum_required (VERSION 3.6)
project (TWidgets)

# Uncomment the next three lines to compile with clang static analyzer
# set(CCC_CXX clang++)
# set(CMAKE_CXX_COMPILER /usr/lib/clang/c++-analyzer)
# set(CMAKE_LINKER clang++)

# Build System Information For Clang-Tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(compile_commands ALL
	COMMAND [ -f ${CMAKE_SOURCE_DIR}/build/compile_commands.json ] && mv ${CMAKE_SOURCE_DIR}/build/compile_commands.json
		${CMAKE_SOURCE_DIR}/compile_commands.json || :)

# Set Output Directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/lib")

# Set Compiler Flags
set(CMAKE_CXX_FLAGS "-std=c++14 -g -Wall")

# Include Directory
include_directories(include)

# Collect Source Files into Variable Names
list(APPEND SYSTEM_MODULE_SOURCES 	"src/system_module/thread_data.cpp"
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
list(APPEND PAINTER_MODULE_SOURCES  "src/painter_module/ncurses_paint_engine.cpp"
									"src/painter_module/painter.cpp"
									"src/painter_module/paint_engine.cpp"
									"src/painter_module/palette.cpp"
									"src/painter_module/glyph.cpp"
									"src/painter_module/brush.cpp"
									"src/painter_module/geometry.cpp"
									"src/painter_module/paint_buffer.cpp"
									)
list(APPEND WIDGET_MODULE_SOURCES 	"src/widget_module/widget.cpp"
									"src/widget_module/layout.cpp"
									"src/widget_module/horizontal_layout.cpp"
									"src/widget_module/border.cpp"
                                    "src/widget_module/vertical_layout.cpp"
                                    "src/widget_module/textbox.cpp"
                                    "src/widget_module/textbox_core.cpp"
                                    "src/widget_module/push_button.cpp"
                                    "src/widget_module/horizontal_scrollbar.cpp"
                                    "src/widget_module/vertical_scrollbar.cpp"
									)

list(APPEND TEST_SOURCES "test/system_module/system_test.cpp"
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

list(APPEND TEST1_SOURCES "test/test1/main.cpp"
						 )

list(APPEND TEST2_SOURCES "test/test2/main.cpp"
						 )

list(APPEND TEST3_SOURCES "test/test3/main.cpp"
						 )

list(APPEND TEST4_SOURCES "test/test4/main.cpp"
						 )

list(APPEND TEST5_SOURCES "test/test5/main.cpp"
						 )

list(APPEND TEST6_SOURCES "test/test6/main.cpp"
						 )

list(APPEND TEST7_SOURCES "test/test7/main.cpp"
						 )

list(APPEND TEST8_SOURCES "test/test8/main.cpp"
						 )

# Create a Static Library for MCurses
add_library(twidgets STATIC ${SYSTEM_MODULE_SOURCES}
						   ${PAINTER_MODULE_SOURCES}
						   ${WIDGET_MODULE_SOURCES}
 						   )

# Create Executables with the Sources
add_executable(tests ${TEST_SOURCES})
add_executable(test1 ${TEST1_SOURCES})
add_executable(test2 ${TEST2_SOURCES})
add_executable(test3 ${TEST3_SOURCES})
add_executable(test4 ${TEST4_SOURCES})
add_executable(test5 ${TEST5_SOURCES})
add_executable(test6 ${TEST6_SOURCES})
add_executable(test7 ${TEST7_SOURCES})
add_executable(test8 ${TEST8_SOURCES})

# Link libraries to executables
target_link_libraries(tests twidgets gtest gtest_main ncurses)
target_link_libraries(test1 twidgets ncurses)
target_link_libraries(test2 twidgets ncurses)
target_link_libraries(test3 twidgets ncurses)
target_link_libraries(test4 twidgets ncurses)
target_link_libraries(test5 twidgets ncurses)
target_link_libraries(test6 twidgets ncurses)
target_link_libraries(test7 twidgets ncurses)
target_link_libraries(test8 twidgets ncurses)

# Install Static Library
install(TARGETS twidgets ARCHIVE DESTINATION lib)

# Install Headers
install(DIRECTORY include/ DESTINATION include/aml/twidgets)

# Doxygen Target - Doxyfile in project/docs - make docs
find_package(Doxygen)
if(DOXYGEN_FOUND)
add_custom_target(docs
${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/docs/Doxyfile
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)
