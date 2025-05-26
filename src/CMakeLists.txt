set(PHOSPHORUS_LIBRARY_SOURCE
        "${PHOSPHORUS_SOURCE_DIR}/main.cpp"
        "${PHOSPHORUS_SOURCE_DIR}/Gnuplot.cpp"
)

find_package(Boost COMPONENTS process)

add_library(phosphorus STATIC
        ${PHOSPHORUS_LIBRARY_SOURCE}
)
target_include_directories(
        phosphorus
        PUBLIC
        $<BUILD_INTERFACE:${PHOSPHORUS_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)
