cmake_minimum_required (VERSION 3.6)
project (MCurses)

# Run these command in shell to use Clang Static Analyzer(and scan-build)
# This need only be run once.
# ________________________________________________________________________________
# export CCC_CXX=clang++
# cmake -DCMAKE_CXX_COMPILER=/usr/lib/clang/c++-analyzer -DCMAKE_LINKER=clang++ ..
#_________________________________________________________________________________
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
list(APPEND EVENT_MODULE_SOURCES 	#"src/event_module/...cpp"
								 	)
list(APPEND PRINTER_MODULE_SOURCES 	#"src/printer_module/...cpp"
									)
list(APPEND SIGNAL_MODULE_SOURCES 	#"src/signal_module/...cpp"
									)
list(APPEND WIDGET_MODULE_SOURCES 	#"src/widget_module/...cpp"
									)

list(APPEND TEST_SOURCES "test/signal_module/none_test.cpp"
						 "test/signal_module/optional_test.cpp"
						 )

# Create a Static Library for MCurses
# add_library(mcurses_lib STATIC ${EVENT_MODULE_SOURCES}
# 							   ${PRINTER_MODULE_SOURCES}
# 							   ${SIGNAL_MODULE_SOURCES}
# 							   ${WIDGET_MODULE_SOURCES}
# 							   )

# Create Executables with the Sources
add_executable(tests ${TEST_SOURCES})

# Link libraries to executables
target_link_libraries(tests gtest gtest_main)	#mcurses_lib

# Doxygen Target - Doxyfile in project/doc - make doc
find_package(Doxygen)
if(DOXYGEN_FOUND)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/doc/Doxyfile
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc
COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)