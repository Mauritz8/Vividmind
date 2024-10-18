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
    src/mailbox_board/mailbox_board.cpp
    src/mailbox_board/square.cpp
    src/mailbox_board/draw.cpp
    src/mailbox_board/playmove.cpp
    src/mailbox_board/move_gen.cpp
    src/bitboards_board/bitboards_board.cpp
    src/bitboards_board/bits.cpp
    src/bitboards_board/move_gen.cpp
    src/bitboards_board/move_gen_lookup_tables.cpp
    src/evaluation/evaluation.cpp
    src/search/search_defs.cpp
    src/search/time_management.cpp
    src/search/search.cpp
    src/board.cpp
    src/piece.cpp
    src/fen.cpp
    src/move.cpp
    src/perft.cpp
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
