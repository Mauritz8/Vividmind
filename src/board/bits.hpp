#pragma once

#include <stdint.h>
#include <string>

namespace bits {
uint64_t get(uint64_t bits, int n);
void set(uint64_t &bits, int n);
void unset(uint64_t &bits, int n);
int popLSB(uint64_t &bits);
std::string to_str(uint64_t bits);
uint64_t reverse(uint64_t bits);
} // namespace bits
