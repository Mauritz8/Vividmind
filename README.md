# Vividmind
Vividmind is a computer program that can play chess, something also known as a chess engine.

## How strong is it?
The best chess engines have since long surpassed the level of human play.
Vividmind on the other hand pales in comparison and the best chess players would easily beat it.
However, that doesn't mean that it's easy to beat, quite the opposite actually. Unless you're a very advanced player, Vividmind will be a serious challenge.

## How do I play against it?
Vividmind is not meant to be used as a standalone program because it doesn't ship with a graphical user interface. 
Instead it implements the Universal Chess Interface (UCI) which enables it to communicate with existing user interfaces.
Today, all major chess applications have support for the UCI protocol, so to play against Vividmind you simply need to install your favorite chess application and configure it to use Vividmind.
However, the easiest way to play against Vividmind is through lichess.org!
Vividmind has a bot account on Lichess, so to play against it you simply need to go to it's profile page [lichess.org/@/Vividmind](https://lichess.org/@/Vividmind) and send it a challenge.

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
