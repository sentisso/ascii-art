cmake_minimum_required(VERSION 3.20)
include(CMakePrintHelpers)

set(project_name ASCII)
set(main_file src/main.cpp)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -pedantic -fsanitize=address")
set(CMAKE_CXX_STANDARD 17)

project(${project_name})

include_directories(src/)

# get project name from path
get_filename_component(main_path "${main_file}" PATH)

# get global path of the project
string(CONCAT main_path "${CMAKE_SOURCE_DIR}/${main_path}")

# RELATIVE ain't fuckin working when searching in subdirs for some reason...
file(GLOB_RECURSE project_files "${main_path}/*.cpp")
file(GLOB_RECURSE project_header_files "${main_path}/*.h")
list(APPEND project_files ${project_header_files})

# convert each file path to relative
set(project_sources "")
foreach( project_file ${project_files} )
    string(REPLACE "${CMAKE_SOURCE_DIR}/" "" project_file "${project_file}")
    list(APPEND project_sources ${project_file}) # so it works in add_executable
endforeach( project_file ${project_files} )

# add project .cpp and .h files
add_executable( ${project_name} ${project_sources} )