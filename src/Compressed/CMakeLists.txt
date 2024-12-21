cmake_minimum_required(VERSION 3.5)
project(ImageLibraryV2-Compressed)

file(GLOB SOURCES "*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ../../include/ ../../external/GArgs/include/ ../../external/GParsing/include/)
target_link_libraries(${PROJECT_NAME} PRIVATE ImageLibraryV2-Core GArgs GParsing-HTTP)
