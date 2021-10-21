cmake_minimum_required (VERSION 3.14)

project(dsproject)

add_subdirectory(heap)
# add_subdirectory(list)
add_subdirectory(threadpool)
add_subdirectory(tree)
# add_subdirectory(wordlist)

add_library(foo STATIC foo.c)

install(TARGETS foo DESTINATION .)

set(LIB_SRC
    "${dsproject_SOURCE_DIR}/wordlist/wordlist.c"
)

set(LIB_HDRS 
    "${dsproject_SOURCE_DIR}/include/wordlist.h"
)

add_library(wordlist STATIC ${LIB_SRC})
target_include_directories(wordlist PUBLIC "./include")
target_link_libraries(wordlist PRIVATE libtree)

install(TARGETS wordlist DESTINATION .)
