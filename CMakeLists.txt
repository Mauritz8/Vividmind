cmake_minimum_required(VERSION 3.14)
project(Vividmind)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG 10.2.0
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)


set(COMMON_SOURCES
    src/board/board.cpp
    src/board/bits.cpp
    src/board/move_gen.cpp
    src/board/masks.cpp
    src/evaluation/evaluation.cpp
    src/engine/time_management.cpp
    src/engine/search.cpp
    src/engine/engine.cpp
    src/engine/command.cpp
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
target_compile_options(vividmind PUBLIC -O3 -march=native -flto)
target_link_options(vividmind PUBLIC -flto)

add_executable(test
    tests/tests.cpp
    ${COMMON_SOURCES}
)
target_include_directories(test PRIVATE
    tests/
    src/
)
target_link_libraries(test GTest::gtest_main fmt::fmt)
target_compile_options(test PUBLIC)
