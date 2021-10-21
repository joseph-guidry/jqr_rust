cmake_minimum_required(VERSION 3.13)

project(ds_tree_proj C)

set(LIB "tree")
set(LIB_SRC
    "tree.c"
    "trie.c"
)

set(LIB_HDRS 
    "${dsproject_SOURCE_DIR}/include/tree.h"
    "${dsproject_SOURCE_DIR}/include/trie.h"
)

add_library(${LIB} STATIC ${LIB_SRC} ${LIB_HDRS})
target_include_directories(${LIB} PUBLIC "../include")

# source_group(
#     TREE "${dsproject_SOURCE_DIR}/include"
#     PREFIX "Header Files"
#     FILES ${LIB_HDRS}
# )