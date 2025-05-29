set(PHOSPHORUS_LIBRARY_SOURCE
        "${PHOSPHORUS_SOURCE_DIR}/main.cpp"
        "${PHOSPHORUS_SOURCE_DIR}/Gnuplot.cpp"
        "${PHOSPHORUS_SOURCE_DIR}/Animate.cpp"
)

find_package(Boost REQUIRED COMPONENTS process)
find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)

add_library(phosphorus STATIC
        ${PHOSPHORUS_LIBRARY_SOURCE}
)

target_link_libraries(
        phosphorus
        PUBLIC
        Boost::process
        morphologica
        range-v3
        OpenMP::OpenMP_CXX
        opencv_core
        opencv_imgproc
        opencv_videoio
        opencv_video
)

target_include_directories(
        phosphorus
        PUBLIC
        $<BUILD_INTERFACE:${PHOSPHORUS_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)
