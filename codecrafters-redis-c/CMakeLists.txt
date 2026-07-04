cmake_minimum_required(VERSION 3.13)

project(codecrafters-redis)

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

list(REMOVE_ITEM SOURCE_FILES 
  "${CMAKE_CURRENT_LIST_DIR}/src/main.c"
  "${CMAKE_CURRENT_LIST_DIR}/src/htable_main.c")

message("SOURCE_FILES ${SOURCE_FILES}")

set(CMAKE_C_STANDARD 99) # Enable the C99 standard

add_executable(redis src/main.c ${SOURCE_FILES})

add_executable(htable src/htable_main.c ${SOURCE_FILES})