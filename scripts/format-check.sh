#!/bin/bash

find ./src ./tests -name '*.cpp' \
      -not -path './src/board/bits.cpp' \
      -o -name '*.hpp' \
      -not -path './src/defs.hpp' \
      -not -path './src/mailbox_board/mailbox_board_defs.hpp' \
      -not -path './src/evaluation/evaluation.hpp' \
      | xargs clang-format --style=file --dry-run --Werror
