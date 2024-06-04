# IFJ23 (subset of Swift) Compiler 

## Description
This is a project for the subject IFJ (Formal Languages and Compilers) at FIT BUT. The goal of the project is to
create a simple compiler for the IFJ23 source language (a subset of Swift). 
The compiler is implemented in C and generates code for the IFJcode23 interpreter.

## Project structure
- `src/` - source code of the compiler
- `src/parts/` - source code of the compiler divided into parts
- `src/utils/` - utility functions for the compiler
- `src/dokumentace/` - documentation of the project (Czech language)

### How to build
To build the project (using CMake), run the following commands in the root directory of the project:
```shell
mkdir build
cd build
cmake ..
make
```

### Example code
Example code written in the IFJ23 language can be found in the `example.ifj23` file in the project root directory.
This code is used to test the compiler. This code is a simple program that renders (simple raytrace) the ASCII 
image of three wheels.

### How to run the compiler
To run the compiler, use the following command in the project root directory:
```shell
./build/compiler < example.ifj23 > example.ifjcode
```
Compiler reads the source code from the standard input and writes the generated code to the standard output.
In case of an error, the application will return a non-zero exit code.

### How to run resulting code
To run the generated code, use the IFJcode23 interpreter (x86-64 systems only). The interpreter can be found in the `bin` directory. 
To run the interpreter, use the following command in the project root directory:
```shell
./bin/ic23int example.ifjcode
```