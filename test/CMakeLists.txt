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
add_executable(test_cppurses_system EXCLUDE_FROM_ALL
    # system/system_test.cpp
    # system/object_test.cpp
    # system/event_loop_test.cpp
    # system/event_test.cpp
    # system/abstract_event_dispatcher_test.cpp
    # system/thread_data_test.cpp
    # system/posted_event_queue_test.cpp
    # system/posted_event_test.cpp
    # system/ncurses_event_dispatcher_test.cpp
)

add_executable(test_cppurses_widget EXCLUDE_FROM_ALL
    # widget/widget_test.cpp
)

add_executable(test_cppurses_painter EXCLUDE_FROM_ALL
    # painter/glyph_test.cpp
    # painter/glyph_string_test.cpp
    # painter/brush_test.cpp
    # painter/palette_test.cpp
    # painter/glyph_matrix_test.cpp
)

# CREATE TESTS
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
target_link_libraries(test_cppurses_system PRIVATE libcppurses gtest)
target_link_libraries(test_cppurses_widget PRIVATE libcppurses gtest)
target_link_libraries(test_cppurses_painter PRIVATE libcppurses gtest)

target_compile_features(test_cppurses_system PRIVATE cxx_std_14)
target_compile_features(test_cppurses_widget PRIVATE cxx_std_14)
target_compile_features(test_cppurses_painter PRIVATE cxx_std_14)

add_custom_target(tests
    DEPENDS
        test_cppurses_system
        test_cppurses_widget
        test_cppurses_painter
)

add_test(test_cppurses_system test_cppurses_system)
add_test(test_cppurses_widget test_cppurses_widget)
add_test(test_cppurses_painter test_cppurses_painter)
