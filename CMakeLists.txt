cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(cppursesproject LANGUAGES CXX)
include(GNUInstallDirs)
enable_testing()

set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src/libs/Optional)
add_subdirectory(src/libs/Signals)
add_subdirectory(src/CPPurses)
