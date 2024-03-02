cmake_minimum_required (VERSION 3.5)
project(hw3)

set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -Wshadow -Werror -D_XOPEN_SOURCE=500")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wshadow -Werror -Wno-sign-compare -D_XOPEN_SOURCE=500")

include_directories("${PROJECT_SOURCE_DIR}/include")

# build a dynamic library called libblock_store.so
# note that the prefix lib will be automatically added in the filename.

# Need to add src files to library
add_library(block_store SHARED src/block_store.c src/bitmap.c)

# Specify output name of share
set_target_properties(block_store PROPERTIES OUTPUT_NAME libblock_store SUFFIX ".so")


# make an executable
add_executable(${PROJECT_NAME}_test test/tests.cpp src/block_store.c)
target_compile_definitions(${PROJECT_NAME}_test PRIVATE)
target_link_libraries(${PROJECT_NAME}_test gtest pthread block_store)
