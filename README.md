# Vividmind
Vividmind is a chess engine (a computer program that can play chess).

## How strong is it?
It's not very strong in chess engine terms, but it's a tough opponent even for 
2000+ elo players.

## How do I play against it?
Send it a challenge on Lichess: [lichess.org/@/Vividmind](https://lichess.org/@/Vividmind).
Vividmind implements the Universal Chess Interface (UCI) protocol so you can
also play against it offline in your favorite chess application.

## Build instructions
First, you need to create the build directory if it doesn't already exist: 
```
mkdir build 
```

To build the program, enter the build directory and compile the code using CMake: 
```
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```
The program will be generated in the output binary `vividmind`.
