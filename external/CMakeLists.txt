cmake_minimum_required(VERSION 3.14)
project(NCurses)

set(LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ncurses-snapshots")

set(LIB_FILE "${LIB_PATH}/output/lib/libncursesw.a")

set(LIB_HEADER_FOLDER "${LIB_PATH}/output/include")

file(MAKE_DIRECTORY "${LIB_PATH}/output/include")


# Build NCurses library from autoconf when nc target is built
add_custom_command(
    OUTPUT ${LIB_FILE}

    COMMAND "${LIB_PATH}/configure" "--prefix=${LIB_PATH}/output"
            --without-shared --with-normal --without-debug --without-ada
            --enable-widec --enable-ext-colors --enable-ext-mouse
            --without-cxx-binding --without-manpages --without-tests
            &&
            make install

    WORKING_DIRECTORY ${LIB_PATH}
)

add_custom_target(build_ncurses
    DEPENDS ${LIB_FILE}
)

# Create ncurses cmake target
add_library(nc STATIC IMPORTED GLOBAL)
add_dependencies(nc build_ncurses)

set_target_properties(nc
    PROPERTIES
    IMPORTED_LOCATION ${LIB_FILE}
    INTERFACE_INCLUDE_DIRECTORIES ${LIB_HEADER_FOLDER}
)

target_link_libraries(nc INTERFACE ${CMAKE_DL_LIBS})
