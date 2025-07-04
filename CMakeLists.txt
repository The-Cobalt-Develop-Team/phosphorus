cmake_minimum_required(VERSION 3.28)
project(phosphorus LANGUAGES CXX)

if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.30)
    cmake_policy(SET CMP0167 NEW)
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-D_WIN32_WINNT=0x0601) # Set minimum Windows version to Windows 7
add_definitions(-DWIN32_LEAN_AND_MEAN) # Exclude rarely-used Windows headers

include(cmake/CMakeUtilities.cmake)

get_max_cxx_standard(PHOSPHORUS_MAX_CXX_STANDARD)
message(STATUS "Maximum C++ Standard Supported: ${PHOSPHORUS_MAX_CXX_STANDARD}")

set(PHOSPHORUS_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PHOSPHORUS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PHOSPHORUS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PHOSPHORUS_THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
set(PHOSPHORUS_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")
set(PHOSPHORUS_EXAMPLES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/examples")

list(INSERT CMAKE_MODULE_PATH 0 "${PHOSPHORUS_ROOT_DIR}/cmake")

phosphorus_option(PHOSPHORUS_BUILD_TESTS "Build tests" ON)
phosphorus_option(PHOSPHORUS_BUILD_EXAMPLES "Build examples" ON)

add_subdirectory("${PHOSPHORUS_THIRD_PARTY_DIR}")

add_subdirectory("${PHOSPHORUS_SOURCE_DIR}")

if (PHOSPHORUS_BUILD_TESTS)
    add_subdirectory("${PHOSPHORUS_TEST_DIR}")
endif ()

if (PHOSPHORUS_BUILD_EXAMPLES)
    add_subdirectory("${PHOSPHORUS_EXAMPLES_DIR}")
endif ()
