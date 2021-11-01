cmake_minimum_required(VERSION 3.13)

project(ifscanner C)

set(IF_SRCS
    scanner.c
)

add_executable(ifscan ${IF_SRCS})
target_link_libraries(ifscan PRIVATE ev)
target_include_directories(ifscan PUBLIC "${CMAKE_SOURCE_DIR}/deps/libev-git/")