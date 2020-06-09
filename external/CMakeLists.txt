cmake_minimum_required(VERSION 3.14)
project(NCurses)

SET(LIB_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/ncurses-snapshots/output/lib/libncursesw.a)

SET(LIB_HEADER_FOLDER
    ${CMAKE_CURRENT_SOURCE_DIR}/ncurses-snapshots/output/include)


# Build NCurses library from autoconf when nc target is built
add_custom_command(
    OUTPUT ${LIB_FILE}

    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/ncurses-snapshots/configure"
            "--prefix=${CMAKE_CURRENT_SOURCE_DIR}/ncurses-snapshots/output"
            --without-shared --with-normal --without-debug --without-ada
            --enable-widec --enable-ext-colors --enable-ext-mouse
            --without-cxx-binding --without-manpages --without-progs
            --without-tack --without-tests
            && make install

    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ncurses-snapshots
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
