cmake_minimum_required (VERSION 3.14)

project(dsproject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

option(WITH_LIBEV "Compile with support for libev" OFF)
option(WITH_THREADPOOL "Compile with support for threadpool" OFF)

add_subdirectory(deps/libev-git)
add_subdirectory(heap)
add_subdirectory(tree)

if(WITH_THREADPOOL)
    message("THIS IS A SUCCESSFUL DEFINE FROM build.rs")
    add_subdirectory(threadpool)
endif()

add_subdirectory(wordlist)
add_subdirectory(ifscanner)

