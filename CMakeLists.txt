cmake_minimum_required(VERSION 3.14)
project(chess-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(COMMON_SOURCES
    src/board.cpp
    src/utils.cpp
    src/move.cpp
    src/move_generator.cpp
    src/piece.cpp
    src/square.cpp
    src/engine/engine.cpp
    src/engine/psqt.cpp
    src/engine/uci.cpp
)

add_executable(vividmind 
    src/main.cpp
    ${COMMON_SOURCES}
)
target_include_directories(vividmind PRIVATE
    include
    include/engine
)


add_executable(tests
    tests/src/test_move_generator.cpp
    ${COMMON_SOURCES}
)
target_include_directories(tests PRIVATE
    include
)
