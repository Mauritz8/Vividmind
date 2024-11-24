# Vividmind

Vividmind is a work-in-progress chess engine written in C++. It adheres to the UCI protocol, making it compatible with various chess graphical user interfaces.

## Features

### Engine
* Bitboard board representation
* UCI-protocol

### Search
* Alpha-Beta
* Iterative Deepening
* Quiescence Search
* Check Extensions
* MVV-LVA

### Evaluation
* Material
* Piece-Square Tables


## Build instructions
Make sure you have CMake installed, and then run the following commands from the root directory of the project:
```
mkdir build 
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```
