# MAS-Lang Compiler: LLVM-Based Implementation

## Compiler Project (Spring 2024)

This is a comprehensive compiler course project built using the **LLVM infrastructure**. The project was executed in two main phases:
1. **Phase 1: Compiler Frontend & Backend** We designed and implemented a compiler for a hypothetical programming language (MAS-Lang). This phase focused on building the lexer, parser, and code generation to produce LLVM Intermediate Representation (IR).
2. **Phase 2: Compiler Optimization** We implemented a custom compiler optimization pass for **Loop Unrolling**. This pass supports both **complete and partial unrolling** for both `for` and `while` loops, significantly improving the performance of the generated code.

## Language Features

The hypothetical language supports key features including:

* **Data Types:** `int` and `bool`.
* **Variable Definition:** Multiple definitions and optional initial assignment.
* **Operators:** Complex arithmetic, relational, logical, and compound assignment operators (`+=`, `-=`, etc.).
* **Control Flow:** `if/else if/else`, `while`, and `for` loops.
* **Unary Operators:** Increment (`x++`) and Decrement (`x--`).

## How to run?
This project uses CMake for building.

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ cd code
$ ./MAS-Lang "int a;"
````

## Sample inputs
```
int a = 0, b = -1, c; 
bool t = true, f = false, d; 
```
```
x = -12 * (y + z^2) / 6; 
```
```
if ( x > 10 ) { 
      y = x; 
} 
else if ( 5 < x and x < 10 ) { 
      y = 2 * x; 
} 
else { 
     y = x / 2; 
} 
```
```
while ( c ) { 
     y += x; 
     if ( y > 21 ) { 
         c = false; 
     } 
} 
```
```
int  i; 
for  ( i = 0 ; i < n ; i += 2 ) {  } 
```

