cmake_minimum_required(VERSION 3.15)
project(ImageLibraryV2-Server)

file(GLOB SOURCES "*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ../../include/)
target_link_libraries(${PROJECT_NAME} PRIVATE ImageLibraryV2-Core)
target_compile_definitions(${PROJECT_NAME} PRIVATE -DCLONE_URI="https://github.com/GustavJones/ImageLibraryV2-Website.git")
