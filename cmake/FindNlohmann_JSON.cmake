# FindNlohmann_JSON.cmake

if (nlohmann_json_FOUND)
    return()
endif ()

set(nlohmann_json_ROOT_DIR "" CACHE PATH "Root directory for nlohmann_json")

# Look for the header file
find_path(nlohmann_json_INCLUDE_DIR
        NAMES nlohmann/json.hpp
        HINTS
        ${nlohmann_json_ROOT_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/../include
        /usr/include
        /usr/local/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(nlohmann_json
        REQUIRED_VARS nlohmann_json_INCLUDE_DIR
        VERSION_VAR nlohmann_json_VERSION
)

if (nlohmann_json_FOUND)
    add_library(nlohmann_json INTERFACE)
    target_include_directories(nlohmann_json INTERFACE
            ${nlohmann_json_INCLUDE_DIR}
    )
else ()
    message(FATAL_ERROR "Could not find nlohmann_json library")
endif ()
