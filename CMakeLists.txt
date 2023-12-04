# https://github.com/vsaltxx/IFJ_PRJ

cmake_minimum_required(VERSION 3.10)
project(IFJ_PRJ C)

set(CMAKE_C_STANDARD 11)

include_directories(scr/utils scr/parts src scr/parts/scanner scr/parts/parser src/parts/codegen)

add_compile_options(-Wall -Wextra -std=c11 -g)

add_executable(compiler
        scr/main.c
        scr/parts/scanner/scanner.c
        scr/macros.h
        scr/parts/parts.h
        scr/utils/source_file.c
        scr/utils/string_util.c
        scr/parts/parser/parser.c
        scr/parts/scanner/scanner.c
        scr/utils/lists.c
        scr/parts/scanner/token.c
        scr/parts/parser/expr_parser.c
        scr/parts/parser/expr_parser.h
        scr/utils/stack.c
        scr/utils/stack.h
        scr/parts/parser/symtable.c
        scr/parts/parser/symtable.c
        scr/utils/memory.c
        scr/utils/memory.h
        scr/parts/codegen/codegen.c
        scr/parts/codegen/codegen.h)