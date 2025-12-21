#!/bin/bash
find src/ tests/ -name '*.cpp' -o -name '*.hpp' | xargs clang-format --verbose --style=file -i
