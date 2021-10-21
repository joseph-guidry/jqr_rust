cmake_minimum_required(VERSION 3.13)

project(ds_wordlist_proj C)

set(LIB_SRC
    "wordlist.c"
)

set(LIB_HDRS 
    "${dsproject_SOURCE_DIR}/include/wordlist.h"
)

add_library(wordlist STATIC ${LIB_SRC})
target_include_directories(wordlist PUBLIC "../include")
target_link_libraries(wordlist PRIVATE libtree)

# source_group(
#     TREE "${dsproject_SOURCE_DIR}/include"
#     PREFIX "Header Files"
#     FILES ${LIB_HDRS}
# )

