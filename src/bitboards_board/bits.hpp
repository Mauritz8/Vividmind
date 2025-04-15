#pragma once

#include <optional>
#include <stdint.h>
#include <string>

namespace bits {
uint64_t get(uint64_t bits, int n);
void set(uint64_t &bits, int n);
void unset(uint64_t &bits, int n);
std::optional<int> popLSB(uint64_t &bits);
std::string to_string(uint64_t bits);
uint64_t reverse(uint64_t bits);
int nr_bits_set(uint64_t bits);
} // namespace bits
