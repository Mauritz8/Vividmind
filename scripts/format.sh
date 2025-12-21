#!/bin/bash

find ./src ./tests \( -name '*.cpp' -o -name '*.hpp' \) \
      -not -path './src/board/bits.cpp' \
      -not -path './src/defs.hpp' \
      -not -path './src/evaluation/evaluation.hpp' \
      | xargs -t clang-format --style=file -i \
