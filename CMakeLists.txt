cmake_minimum_required(VERSION 3.16.3)

project(Phosphorus)

# Compile options
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_FLAGS "-fno-asm -fno-ms-extensions -std=c++17 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Og")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Show message
message("Phosphorus - A Physics Engine for Movement Analysis")
if(${CMAKE_BUILD_TYPE} STREQUAL DEBUG)
    message("Optional:-fno-asm -std=c++17 -Wall -g -Og, Type:Debug")
else(${CMAKE_BUILD_TYPE} STREQUAL RELEASE)
    message("Optional:-fno-asm -std=c++17 -Wall -Ofast, Type:Release")
endif(${CMAKE_BUILD_TYPE} STREQUAL DEBUG)

include_directories(include/ include/JSONResolver)
add_subdirectory(src/)

# Install options

# Library
#install(TARGETS 
#        ARCHIVE DESTINATION lib)

# Header files
# install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
#         DESTINATION include)

# Executable files
# install(TARGETS Lithium
#         RUNTIME DESTINATION bin)
