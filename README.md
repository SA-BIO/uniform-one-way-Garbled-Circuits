# Uniform one-way Garbled Circuits

## Dependencies
This code utilizes SIMD instruction and requires an architecture compatible with such operations. 
The only other dependency is the haraka hash function which is **already included** in the code.

## Compilation
This code was compiled in Windows using Visual Studio 2022 and the **Microsoft Visual C++** compiler. 
The code should work on any OS using any compiler. For adequate performance use the O2 flag when compiling. Some additional flags may be necessary to enable use of SIMD instructions.
