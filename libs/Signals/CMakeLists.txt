cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(libsignals LANGUAGES CXX)
include(GNUInstallDirs)
enable_testing()
add_compile_options(-std=c++14)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(TEST_SOURCES 
    "test/slot_test.cpp"
    "test/slot_base_test.cpp"
    "test/connection_test.cpp"
    "test/connection_impl_test.cpp"
    "test/signal_test.cpp"
    "test/signal_impl_test.cpp"
    "test/shared_connection_block_test.cpp"
    "test/optional_last_value_test.cpp"
    )

add_executable(testsignals ${TEST_SOURCES})

target_include_directories(testsignals PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

find_package(GTest REQUIRED)
if (GTEST_FOUND)
    target_link_libraries(testsignals ${GTEST_BOTH_LIBRARIES})
endif()

add_test(testsignals testsignals)

# Build System Info For Clang-Tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(compile_commands_signals ALL
    COMMAND [ -f ${CMAKE_SOURCE_DIR}/build/compile_commands.json ] && mv
    	${CMAKE_SOURCE_DIR}/build/compile_commands.json
    	${CMAKE_SOURCE_DIR}/compile_commands.json || :)

# Doxygen Target
find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(docs_signals
    ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/docs/Doxyfile
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()
