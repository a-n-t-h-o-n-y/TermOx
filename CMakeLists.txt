cmake_minimum_required (VERSION 3.6)
project (MCurses)

# Run these command in shell before cmake to use Clang Static Analyzer
# This need only be run once.
#__________________________________________________________________________
# export CCC_CXX=clang++
# cmake -DCMAKE_CXX_COMPILER=/usr/lib/clang/c++-analyzer -DCMAKE_LINKER=clang++ ..
#__________________________________________________________________________
# The path to c++-analyzer may be different on your machine.

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
list(APPEND SYSTEM_MODULE_SOURCES 	#"src/system_module/...cpp"
								 	)
list(APPEND PRINTER_MODULE_SOURCES 	#"src/printer_module/...cpp"
									)
list(APPEND SIGNAL_MODULE_SOURCES 	"src/signal_module/slot_base.cpp"
									"src/signal_module/connection.cpp"
									"src/signal_module/shared_connection_block.cpp"
									)
list(APPEND WIDGET_MODULE_SOURCES 	#"src/widget_module/...cpp"
									)

list(APPEND TEST_SOURCES "test/signal_module/none_test.cpp"
						 "test/signal_module/optional_test.cpp"
						 "test/signal_module/optional_last_value_test.cpp"
						 "test/signal_module/slot_test.cpp"
						 "test/signal_module/slot_base_test.cpp"
						 "test/signal_module/connection_test.cpp"
						 "test/signal_module/connection_impl_test.cpp"
						 "test/signal_module/signal_test.cpp"
						 "test/signal_module/signal_impl_test.cpp"
						 "test/signal_module/shared_connection_block_test.cpp"
						 )

# Create a Static Library for MCurses
add_library(mcurses_lib STATIC #${SYSTEM_MODULE_SOURCES}
# 							   ${PRINTER_MODULE_SOURCES}
 							   ${SIGNAL_MODULE_SOURCES}
# 							   ${WIDGET_MODULE_SOURCES}
 							   )

# Create Executables with the Sources
add_executable(tests ${TEST_SOURCES})

# Link libraries to executables
target_link_libraries(tests mcurses_lib gtest gtest_main)

# Doxygen Target - Doxyfile in project/doc - make doc
find_package(Doxygen)
if(DOXYGEN_FOUND)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/doc/Doxyfile
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc
COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)