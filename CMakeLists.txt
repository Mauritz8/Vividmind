cmake_minimum_required(VERSION 3.14)
project(Vividmind)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-O3 -march=native -flto)
add_link_options(-flto)


include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG 10.2.0
)
FetchContent_MakeAvailable(fmt)


set(COMMON_SOURCES
    src/board/draw.cpp
    src/board/fen.cpp
    src/board/playmove.cpp
    src/board.cpp
    src/evaluation/evaluation.cpp
    src/move_gen.cpp
    src/search/defs.cpp
    src/search/time_management.cpp
    src/search.cpp
    src/move.cpp
    src/perft.cpp
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
target_link_libraries(vividmind fmt::fmt)


add_executable(tests
    tests/test_move_gen.cpp
    ${COMMON_SOURCES}
)
target_include_directories(tests PRIVATE
    tests/
    src/
)
target_link_libraries(tests fmt::fmt)
