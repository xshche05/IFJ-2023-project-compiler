# https://github.com/vsaltxx/IFJ_PRJ

cmake_minimum_required(VERSION 3.26)
project(IFJ_PRJ C)

set(CMAKE_C_STANDARD 99)

include_directories(scr/utils scr/parts)

add_executable(IFJ_PRJ
        scr/main.c
        scr/parts/lexer/lexer.c
        scr/utils/file_util.c
        scr/utils/string_util.c
        scr/parts/syntax/syntax.c)