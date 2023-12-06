#!/bin/bash

COMPILER_PATH=./cmake-build-debug/compiler
INTERPRETER_PATH=./tests/ic23int
SOURCE_PATH=./test.swift
ASM_PATH=./test.s

$COMPILER_PATH < $SOURCE_PATH > $ASM_PATH
code=$?
if [ $code -ne 0 ]; then
    echo "Compilation failed"
    echo "Exit code: $code"
    rm $ASM_PATH
    exit 0
fi
$INTERPRETER_PATH $ASM_PATH
echo ""
rm $ASM_PATH