cmake_minimum_required(VERSION 3.5)
project(ImageLibraryV2-Reload)

file(GLOB SOURCES "*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ../../include/)
