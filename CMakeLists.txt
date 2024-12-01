cmake_minimum_required(VERSION 3.5)
project(ImageLibraryV2)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(external/GArgs)
add_subdirectory(external/GParsing)

add_subdirectory(src)
