# Define a function to declare an option and print its value
function(phosphorus_option opt_name opt_desc opt_default)
    option(${opt_name} "${opt_desc}" ${opt_default})
    message(STATUS "${opt_name}: [${${opt_name}}]")
endfunction()

function(phosphorus_add_example example_name)
    set(example_source "${PHOSPHORUS_EXAMPLES_DIR}/${example_name}.cpp")
    set(example_target "${example_name}")

    add_executable(${example_target} ${example_source})
    target_link_libraries(${example_target} PRIVATE phosphorus)
    set_target_properties(${example_target} PROPERTIES OUTPUT_NAME ${example_name})
    message(STATUS "Added example: ${example_name}")
endfunction()
