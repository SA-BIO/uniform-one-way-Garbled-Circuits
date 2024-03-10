# Uniform one-way Garbled Circuits
This work proposes a silent and efficient Oblivious Transfer expansion protocol tailored to a particular context. The main observation is that, for constructs, specifically Garbled Circuits, where the receiver's input follows a specific distribution and the sender doesn't interact with the output, security demands can be greatly relaxed and decoupled from the Learning Parity with Noise assumption used in similar work. The provided algorithms, which are publicly available, achieve exceptional performance in both execution times and data exchange. More specifically these are capable of generating 10 million Correlated Oblivious Transfers in 70 milliseconds with a constant 12 KB of information transfer.


## Dependencies
This code utilizes SIMD instruction and requires an architecture compatible with such operations. The only other dependency is the [haraka](https://github.com/kste/haraka) hash function which is **already included** in the code.

## Compilation
This code was compiled in Windows using Visual Studio 2022 and the **Microsoft Visual C++** compiler. The code should work on any OS using any compiler. The source files are located in the OneWayGC_OTgen folder. The relevan files are "OneWayGC_OTgen.cpp", "haraka.cpp", "haraka.h" and "utils.h". The other files are Visual Studio configuration files and are not needed if using another program or compiler.

For adequate performance use the **O2 flag** when compiling. Some additional flags may be necessary to enable use of SIMD instructions.

## Executable
An existing executable is avaialable in the x64/Release folder, but for best results we suggest compiling the project on your own for your specific architecture.
