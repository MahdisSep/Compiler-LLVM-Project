# Compiler Project (Spring 2024)
A compiler project based on llvm.

## How to run?
```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ cd code
$ ./MAS-Lang "int a;"
```
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
