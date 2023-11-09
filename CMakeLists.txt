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
    src/search/time_management.cpp
    src/search.cpp
    src/evaluation/evaluation.cpp
    src/evaluation/psqt.cpp
    src/move.cpp
    src/move_generator.cpp
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
    tests/src/test_move_generator.cpp
    ${COMMON_SOURCES}
)
target_include_directories(tests PRIVATE
    src/
)
