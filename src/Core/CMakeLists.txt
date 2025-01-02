cmake_minimum_required(VERSION 3.15)
project(ImageLibraryV2-Core)

file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ../../include/)
