cmake_minimum_required(VERSION 3.14)
project(chess-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# for debugging
# set(CMAKE_BUILD_TYPE Debug)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)


set(COMMON_SOURCES
    src/board.cpp
    src/board_utils.cpp
    src/game_over_conditions.cpp
    src/game_state.cpp
    src/move.cpp
    src/piece.cpp
    src/pos.cpp
    src/square.cpp
    src/engine/engine.cpp
    src/engine/uci.cpp
    src/engine/psqt.cpp
    src/pieces/king.cpp
    src/pieces/pawn.cpp
)

add_executable(chess_engine 
    src/main.cpp
    ${COMMON_SOURCES}
)
target_include_directories(chess_engine PRIVATE
    include
    include/engine
)


enable_testing()

add_executable(unit_tests
    tests/src/test_move_generator.cpp
    ${COMMON_SOURCES}
)
target_include_directories(unit_tests PRIVATE
    include
)
target_link_libraries(
    unit_tests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
