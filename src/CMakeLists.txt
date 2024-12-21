cmake_minimum_required(VERSION 3.5)
project(ImageLibraryV2-Sources)

file(GLOB ImageLibraryV2-Components "*/")

foreach(ImageLibraryV2-Component ${ImageLibraryV2-Components})
  if(IS_DIRECTORY ${ImageLibraryV2-Component})
    add_subdirectory(${ImageLibraryV2-Component})
  endif()
endforeach()
