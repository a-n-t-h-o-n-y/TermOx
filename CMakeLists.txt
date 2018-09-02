cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(libcppurses LANGUAGES CXX)
include(GNUInstallDirs)
# enable_testing()

# set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)
message("Build Type: ${CMAKE_BUILD_TYPE}" )

# ADD SIGNALS LIBRARY
add_subdirectory(libs/Signals)

# ADD CPPURSES LIBRARY
add_subdirectory(src)

# ADD DEMOS
add_subdirectory(demos)

# ADD TESTS
# add_subdirectory(test)

# CLANG TIDY
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(compile_commands ALL
	COMMAND [ -f ${CMAKE_SOURCE_DIR}/build/compile_commands.json ] && mv
        ${CMAKE_SOURCE_DIR}/build/compile_commands.json
		${CMAKE_SOURCE_DIR}/compile_commands.json || :)

# DOXYGEN
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
        COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()
