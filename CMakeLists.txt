cmake_minimum_required(VERSION 3.15)
project(ImageLibraryV2)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)
add_subdirectory(external/TinyEXIF)
add_subdirectory(external/GArgs)
add_subdirectory(external/GParsing)

add_subdirectory(src)
