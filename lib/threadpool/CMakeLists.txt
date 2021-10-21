cmake_minimum_required(VERSION 3.13)

project(ds_tp_proj C)

set(LIB "tp")
set(LIB_SRC
    "threadpool.c"
)

set(LIB_HDRS 
    "${dsproject_SOURCE_DIR}/include/tp.h"
)

add_library(${LIB} STATIC ${LIB_SRC} ${LIB_HDRS})
target_link_libraries(${LIB} PRIVATE pthread libheap)
target_include_directories(${LIB} PUBLIC "../include")

# source_group(
#     TREE "${dsproject_SOURCE_DIR}/include"
#     PREFIX "Header Files"
#     FILES ${LIB_HDRS}
# )