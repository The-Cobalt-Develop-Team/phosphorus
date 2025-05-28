# - Try to find the GLFW library
# Once done, this will define:
#
#  GLFW_FOUND          - system has GLFW
#  GLFW_INCLUDE_DIRS   - the GLFW include directories
#  GLFW_LIBRARIES      - link these to use GLFW
#
# You can optionally specify:
#  GLFW_ROOT_DIR       - root directory where to search for GLFW

set(GLFW3_ROOT_DIR "" CACHE PATH "Root directory where GLFW is installed")

find_path(GLFW3_INCLUDE_DIR
        NAMES GLFW/glfw3.h
        HINTS
        ${GLFW3_ROOT_DIR}/include
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        DOC "The directory where GLFW/glfw3.h resides"
)


find_library(GLFW3_LIBRARY
        NAMES glfw3 glfw
        HINTS
        ${GLFW3_ROOT_DIR}/lib
        ${GLFW3_ROOT_DIR}/lib/x86_64-linux-gnu
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        DOC "The GLFW library"
)

function(detect_GLFW3_version)
    if (NOT GLFW3_INCLUDE_DIR)
        message(WARNING "Cannot find dependency headers")
        return()
    endif ()

    set(version_detector "
#include <cstdio>
#include <GLFW/glfw3.h>

#define STRINGIFY(x) #x
#define EXPAND_STRINGIFY(x) STRINGIFY(x)

#if defined(GLFW_VERSION_MAJOR) && defined(GLFW_VERSION_MINOR) && defined(GLFW_VERSION_REVISION)
    #define GLFW3_VERSION_STRING EXPAND_STRINGIFY(GLFW_VERSION_MAJOR) \".\" \
                                 EXPAND_STRINGIFY(GLFW_VERSION_MINOR) \".\" \
                                 EXPAND_STRINGIFY(GLFW_VERSION_REVISION)
    #define DETECTED_VERSION GLFW3_VERSION_STRING
#elif defined(GLFW_VERSION_STRING)
    #define DETECTED_VERSION GLFW3_VERSION_STRING
#elif defined(GLFW_VERSION)
    #define DETECTED_VERSION GLFW3_VERSION
#else
    #error No version macros found
#endif

int main() {
    printf(\"%s\", DETECTED_VERSION);
    return 0;
}")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/version_detect.cpp "${version_detector}")

    try_compile(COMPILE_SUCCESS
            ${CMAKE_CURRENT_BINARY_DIR}
            SOURCES ${CMAKE_CURRENT_BINARY_DIR}/version_detect.cpp
            CMAKE_FLAGS
            "-DINCLUDE_DIRECTORIES=${GLFW3_INCLUDE_DIR}"
            OUTPUT_VARIABLE COMPILE_OUTPUT
    )

    if (COMPILE_SUCCESS)
        try_run(
                RUN_RESULT
                COMPILE_RESULT
                ${CMAKE_CURRENT_BINARY_DIR}
                SOURCES ${CMAKE_CURRENT_BINARY_DIR}/version_detect.cpp
                CMAKE_FLAGS
                "-DINCLUDE_DIRECTORIES=${GLFW3_INCLUDE_DIR}"
                RUN_OUTPUT_VARIABLE RAW_VERSION
        )

        if (RUN_RESULT EQUAL 0)
            string(STRIP "${RAW_VERSION}" DETECTED_VERSION)
            set(GLFW3_VERSION ${DETECTED_VERSION} PARENT_SCOPE)
        endif ()
    else ()
        message(STATUS "Version detection failed: ${COMPILE_OUTPUT}")
    endif ()
endfunction()

detect_GLFW3_version()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(glfw3
        REQUIRED_VARS GLFW3_LIBRARY GLFW3_INCLUDE_DIR
        FOUND_VAR GLFW3_FOUND
        VERSION_VAR GLFW3_VERSION
        HANDLE_VERSION_RANGE
)

if (GLFW3_FOUND)
    set(GLFW3_INCLUDE_DIRS ${GLFW3_INCLUDE_DIR})
    set(GLFW3_LIBRARIES ${GLFW3_LIBRARY})
    mark_as_advanced(GLFW3_INCLUDE_DIR GLFW3_LIBRARY)

    add_library(glfw3 SHARED IMPORTED)
    set_target_properties(glfw3 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GLFW3_INCLUDE_DIRS}"
            IMPORTED_LOCATION "${GLFW3_LIBRARY}"
            IMPORTED_IMPLIB "${GLFW3_LIBRARY}"
    )
    message(STATUS "Found GLFW3: ${GLFW3_VERSION}")
    message(STATUS "GLFW3 include directories: ${GLFW3_INCLUDE_DIRS}")
    message(STATUS "GLFW3 libraries: ${GLFW3_LIBRARIES}")
endif ()
