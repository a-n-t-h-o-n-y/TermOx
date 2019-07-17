cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(libcppurses LANGUAGES CXX)
include(GNUInstallDirs)
# enable_testing()
message("Build Type: ${CMAKE_BUILD_TYPE}" )

if(${CMAKE_VERSION} VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# ADD SIGNALS LIBRARY
add_subdirectory(libs/Signals)

# ADD CPPURSES LIBRARY
add_subdirectory(src)

# ADD DEMOS
add_subdirectory(demos)

# ADD TESTS
add_subdirectory(test)

# DOXYGEN
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(cppurses_docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
        COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()
