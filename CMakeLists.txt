# https://github.com/vsaltxx/IFJ_PRJ

cmake_minimum_required(VERSION 3.26)
project(IFJ_PRJ C)

set(CMAKE_C_STANDARD 99)

add_executable(IFJ_PRJ
        main.c
        parts/lexer/lexer.c
        utils/file_util.c
        utils/string_util.c
        parts/syntax/syntax.c)


include_directories(./utils ./parts)