option(PHOSPHORUS_USE_MORPHOLOGICA "Use Morphologica for plotting" ON)
if (PHOSPHORUS_USE_MORPHOLOGICA)
    set(MORPHOLOGICA_ROOT_DIR "${PHOSPHORUS_THIRD_PARTY_DIR}/morphologica")
    set(MORPHOLOGICA_BUILD_EXAMPLES OFF)
    set(MORPHOLOGICA_BUILD_TESTS OFF)
    set(MORPHOLOGICA_BUILD_DOCS OFF)
    add_library(morphologica INTERFACE)
    find_package(OpenGL REQUIRED)
    find_package(Freetype REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(nlohmann_json REQUIRED)
    target_include_directories(morphologica INTERFACE
            "${MORPHOLOGICA_ROOT_DIR}"
            "${GLFW3_INCLUDE_DIR}"
    )
    target_link_libraries(
            morphologica
            INTERFACE
            OpenGL::GL
            OpenGL::GLU
            glfw3
            Freetype::Freetype
            nlohmann_json
    )
    message(STATUS "Morphologica library added")
endif ()
