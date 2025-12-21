# Vividmind
Vividmind is a chess engine (a computer program that can play chess).

## How strong is it?
It's not very strong in chess engine terms, but it's a tough opponent even for 
2000+ elo players.

## How do I play against it?
Send it a challenge on Lichess: [lichess.org/@/Vividmind](https://lichess.org/@/Vividmind).
Vividmind implements the Universal Chess Interface (UCI) protocol so you can
also play against it offline in your favorite chess application.

## Development
- Generate build system: `cmake -DCMAKE_BUILD_TYPE=Release -B build/`
- Run engine: `cmake --build build/ -t vividmind && ./build/vividmind`
- Test: `cmake --build build/ -t test && ./build/test --gtest_break_on_failure`
- Format: `./scripts/format.sh # requires clang-format`
