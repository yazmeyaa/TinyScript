cmake_minimum_required(VERSION 3.15)
project(TinyScript C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(list STATIC
        src/utils/list.c
        src/lexer/lexer.c
        src/utils/fs.c
)

target_include_directories(list PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)


add_executable(main main.c)
target_link_libraries(main PRIVATE list)