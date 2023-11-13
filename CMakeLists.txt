# https://github.com/vsaltxx/IFJ_PRJ

cmake_minimum_required(VERSION 3.10)
project(IFJ_PRJ C)

set(CMAKE_C_STANDARD 99)

include_directories(scr/utils scr/parts src scr/parts/scanner scr/parts/parser)

add_compile_options(-Wall -Wextra -std=c99 -g)

add_executable(IFJ_PRJ
        scr/main.c
        scr/parts/scanner/scanner.c
        scr/utils/file_util.c
        scr/utils/string_util.c
        scr/parts/parser/parser.c
        scr/parts/scanner/scanner.c
        scr/utils/lists.c
        scr/parts/scanner/token.c
        scr/parts/parser/expr_parser.c
        scr/parts/parser/expr_parser.h
        scr/utils/stack.c
        scr/utils/stack.h archive/symtable.c archive/symtable.h)

add_executable(tests
        scr/parts/scanner/scanner.c
        scr/utils/file_util.c
        scr/utils/string_util.c
        scr/parts/parser/parser.c
        scr/utils/lists.c
        scr/parts/scanner/scanner.c
        scr/parts/scanner/token.c)

add_executable(sym_test
        tests/sym_test.c
        archive/symtable.c
        scr/utils/stack.c
        scr/utils/string_util.c
        scr/parts/parser/new_symtable.c
)