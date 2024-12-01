cmake_minimum_required(VERSION 3.5)
project(ImageLibraryV2-Server)

file(GLOB SOURCES "*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ../../include/)
target_compile_definitions(${PROJECT_NAME} PRIVATE -DCLONE_URI="https://github.com/GustavJones/ImageLibraryV2-Website.git")
