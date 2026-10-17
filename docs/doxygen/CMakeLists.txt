cmake_minimum_required(VERSION 3.18)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(TermOxReference LANGUAGES NONE)
endif()

find_package(Doxygen 1.18.0 EXACT REQUIRED COMPONENTS doxygen)
get_filename_component(TERMOX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
file(STRINGS "${TERMOX_SOURCE_DIR}/CMakeLists.txt" version_line
     REGEX "^project\\(TermOx VERSION ")
string(REGEX REPLACE ".*VERSION ([0-9.]+).*" "\\1" TERMOX_DOC_VERSION "${version_line}")
set(TERMOX_DOC_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}")
set(TERMOX_DOC_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}")
set(TERMOX_DOC_THEME "${TERMOX_DOC_SOURCE}/vendor/doxygen-awesome")

# Generate the header with this Doxygen version, then apply the theme's documented
# dark-mode extension. No copied Doxygen HTML template needs to be maintained.
execute_process(
    COMMAND "${DOXYGEN_EXECUTABLE}" -w html
            "${TERMOX_DOC_OUTPUT}/header.html"
            "${TERMOX_DOC_OUTPUT}/footer.html"
            "${TERMOX_DOC_OUTPUT}/stock.css"
    RESULT_VARIABLE header_result
    OUTPUT_QUIET
)
if(NOT header_result EQUAL 0)
    message(FATAL_ERROR "Doxygen could not generate its HTML header")
endif()
file(READ "${TERMOX_DOC_OUTPUT}/header.html" header)
string(REPLACE "</head>" [=[
<script src="$relpath^doxygen-awesome-darkmode-toggle.js"></script>
<script>DoxygenAwesomeDarkModeToggle.init();</script>
</head>]=] header "${header}")
file(WRITE "${TERMOX_DOC_OUTPUT}/header.html" "${header}")
configure_file(Doxyfile.in "${TERMOX_DOC_OUTPUT}/Doxyfile" @ONLY)

add_custom_target(TermOx.docs
    COMMAND "${DOXYGEN_EXECUTABLE}" "${TERMOX_DOC_OUTPUT}/Doxyfile"
    WORKING_DIRECTORY "${TERMOX_SOURCE_DIR}"
    COMMENT "Generating TermOx ${TERMOX_DOC_VERSION} API reference"
    VERBATIM
)
