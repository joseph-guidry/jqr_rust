cmake_minimum_required(VERSION 3.13)

project(ds_heap_proj C)

set(LIB_SRC
    "heap.c"
)

set(LIB_HDRS 
    "${dsproject_SOURCE_DIR}/include/heap.h"
)

add_library(heap STATIC ${LIB_SRC} ${LIB_HDRS})
target_include_directories(heap PUBLIC "../include")

# source_group(
#     TREE "${dsproject_SOURCE_DIR}/c/include"
#     PREFIX "Header Files"
#     FILES ${LIB_HDRS}
# )