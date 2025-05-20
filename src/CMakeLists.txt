set(PHOSPHORUS_LIBRARY_SOURCE "${PHOSPHORUS_SOURCE_DIR}/main.cpp")

add_library(phosphorus STATIC
        ${PHOSPHORUS_LIBRARY_SOURCE}
)
target_include_directories(
        phosphorus
        PUBLIC
        $<BUILD_INTERFACE:${PHOSPHORUS_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)
