cmake_minimum_required(VERSION 3.14)
project(chess-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)


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
target_include_directories(chess_engine PRIVATE
    include
    include/engine
)
target_link_libraries(chess_engine gtest_main)


add_executable(chess_cli_game 
    src/terminal_game.cpp
    ${COMMON_SOURCES}
)
target_include_directories(chess_cli_game PRIVATE
    include
    include/engine
)
target_link_libraries(chess_cli_game gtest_main)
