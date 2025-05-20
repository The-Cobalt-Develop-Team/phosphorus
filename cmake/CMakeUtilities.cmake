# Define a function to declare an option and print its value
function(phosphorus_option opt_name opt_desc opt_default)
    option(${opt_name} "${opt_desc}" ${opt_default})
    message(STATUS "${opt_name}: [${${opt_name}}]")
endfunction()
