# https://github.com/vsaltxx/IFJ_PRJ

cmake_minimum_required(VERSION 3.10)
project(IFJ_PRJ C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(src/utils/ src/parts/ src/ src/parts/scanner/ src/parts/parser/ src/parts/codegen/)

add_compile_options(-Wall -Wextra -std=c99 -g)

add_custom_target(run_code
        DEPENDS compiler
        WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})

add_custom_target(test1
        DEPENDS compiler
        WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}/tests/test1)

add_custom_target(test2
        DEPENDS compiler
        WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}/tests/test2)

add_executable(compiler
        src/main.c
        src/parts/scanner/scanner.c
        src/macros.h
        src/utils/source_file.c
        src/utils/string_util.c
        src/parts/parser/parser.c
        src/parts/scanner/scanner.c
        src/utils/lists.c
        src/parts/scanner/token.c
        src/parts/parser/expr_parser.c
        src/parts/parser/expr_parser.h
        src/utils/stack.c
        src/utils/stack.h
        src/parts/parser/symtable.c
        src/parts/parser/symtable.c
        src/utils/memory.c
        src/utils/memory.h
        src/parts/codegen/codegen.c
        src/parts/codegen/codegen.h)