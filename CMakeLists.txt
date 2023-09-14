cmake_minimum_required(VERSION 3.0)

project(chess-engine)

set(CMAKE_CXX_STANDARD 17)

set(COMMON_SOURCES
    src/board.cpp
    src/game_over_conditions.cpp
    src/game_state.cpp
    src/move.cpp
    src/piece.cpp
    src/square.cpp
    src/engine/engine.cpp
    src/engine/uci.cpp
)

add_executable(chess_engine 
    src/main.cpp
    ${COMMON_SOURCES}
)

add_executable(chess_cli_game 
    src/terminal_game.cpp
    ${COMMON_SOURCES}
)

target_include_directories(chess_engine PRIVATE
    include
    include/engine
)

target_include_directories(chess_cli_game PRIVATE
    include
    include/engine
)
