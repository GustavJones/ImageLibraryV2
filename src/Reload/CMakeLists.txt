cmake_minimum_required(VERSION 3.15)
project(ImageLibraryV2-Reload)

file(GLOB SOURCES "*.cpp")
file(GLOB SUBMODULES LIST_DIRECTORIES true "../../external/*")

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ../../include/ ../../external/stb/ ../../external/TinyEXIF/)

foreach(SUBMODULE ${SUBMODULES})
  target_include_directories(${PROJECT_NAME} PRIVATE ${SUBMODULE}/include/)
endforeach()

target_link_libraries(${PROJECT_NAME} PRIVATE GArgs GParsing-HTTP TinyEXIFstatic)
target_link_libraries(${PROJECT_NAME} PRIVATE ImageLibraryV2-Core)
