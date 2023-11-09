cmake_minimum_required(VERSION 3.14)
project(Vividmind)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(COMMON_SOURCES
    src/board/draw.cpp
    src/board/fen.cpp
    src/board/mailbox.cpp
    src/board/playmove.cpp
    src/board.cpp
    src/evaluation/evaluation.cpp
    src/evaluation/psqt.cpp
    src/move_gen/king.cpp
    src/move_gen/pawn.cpp
    src/move_gen.cpp
    src/search/time_management.cpp
    src/search.cpp
    src/move.cpp
    src/piece.cpp
    src/square.cpp
    src/uci.cpp
    src/utils.cpp
)

add_executable(vividmind 
    src/main.cpp
    ${COMMON_SOURCES}
)
target_include_directories(vividmind PRIVATE
    src/
)


add_executable(tests
    tests/test_move_gen.cpp
    tests/test_positions.cpp
    ${COMMON_SOURCES}
)
target_include_directories(tests PRIVATE
    tests/
    src/
)
