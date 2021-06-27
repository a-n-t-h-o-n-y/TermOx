cmake_minimum_required(VERSION 3.9)
project(TermOx LANGUAGES CXX)

# Build Types
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}
    CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel tsan asan lsan msan ubsan"
    FORCE
)

# ThreadSanitizer
set(CMAKE_CXX_FLAGS_TSAN
    "-fsanitize=thread -g -O1"
    CACHE STRING "Flags used by the C++ compiler during ThreadSanitizer builds."
    FORCE
)

# AddressSanitize
set(CMAKE_CXX_FLAGS_ASAN
    "-fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1"
    CACHE STRING "Flags used by the C++ compiler during AddressSanitizer builds."
    FORCE
)

# LeakSanitizer
set(CMAKE_CXX_FLAGS_LSAN
    "-fsanitize=leak -fno-omit-frame-pointer -g -O1"
    CACHE STRING "Flags used by the C++ compiler during LeakSanitizer builds."
    FORCE
)

# MemorySanitizer
set(CMAKE_CXX_FLAGS_MSAN
    "-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2"
    CACHE STRING "Flags used by the C++ compiler during MemorySanitizer builds."
    FORCE
)

# UndefinedBehaviour
set(CMAKE_CXX_FLAGS_UBSAN
    "-fsanitize=undefined"
    CACHE STRING "Flags used by the C++ compiler during UndefinedBehaviourSanitizer builds."
    FORCE
)

message("Build Type: ${CMAKE_BUILD_TYPE}" )

set(TERMOX_BUILD_TESTS ON CACHE BOOL "Create termox.ui.tests and termox.unit.tests targets")

set(TERMOX_BUILD_DEMOS ON CACHE BOOL "Create demos and readme.demo targets")

# if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#     add_compile_options(-D_GLIBCXX_DEBUG -D_LIBCPP_DEBUG=1)
# endif()

# Add Escape Library
add_subdirectory(external/Escape)

# Add Signals-Light
add_subdirectory(external/signals-light)

if (NOT TARGET Catch2)
    add_subdirectory(external/Catch2 EXCLUDE_FROM_ALL)
endif()

# Add TermOx
add_subdirectory(src)

# Add Demos
if (TERMOX_BUILD_DEMOS)
    add_subdirectory(demos)
endif()

# Add Tests
if (TERMOX_BUILD_TESTS)
    add_subdirectory(tests)
endif()

add_custom_target(
    termox.all
    DEPENDS
        demos
        readme.demo
        termox.ui.tests
        termox.unit.tests
)
