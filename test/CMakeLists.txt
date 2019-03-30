# FIND GTEST
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
find_package(GTest REQUIRED)
add_library(gtest INTERFACE IMPORTED)
set_property(TARGET gtest PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${GTEST_INCLUDE_DIRS})
set_property(TARGET gtest PROPERTY
    INTERFACE_LINK_LIBRARIES ${GTEST_BOTH_LIBRARIES})

# GATHER SOURCES
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
add_executable(cppurses_test EXCLUDE_FROM_ALL
    # system/system_test.cpp
    # system/object_test.cpp
    # system/event_loop_test.cpp
    # system/event_test.cpp
    # system/abstract_event_dispatcher_test.cpp
    # system/thread_data_test.cpp
    # system/posted_event_queue_test.cpp
    # system/posted_event_test.cpp
    # system/ncurses_event_dispatcher_test.cpp
    # widget/widget_test.cpp
    # painter/glyph_test.cpp
    # painter/glyph_string_test.cpp
    # painter/brush_test.cpp
    # painter/palette_test.cpp
    # painter/glyph_matrix_test.cpp
)

# CREATE TESTS
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
target_link_libraries(cppurses_test PRIVATE cppurses gtest)

if(${CMAKE_VERSION} VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD 14)
else()
    target_compile_features(cppurses_test PRIVATE cxx_std_14)
endif()

add_test(cppurses_test cppurses_test)
