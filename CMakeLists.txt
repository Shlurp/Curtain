cmake_minimum_required(VERSION 3.10)

project(curtain)

file(GLOB_RECURSE SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

message("SOURCE: ${SOURCE}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_library(curtain SHARED ${SOURCE})