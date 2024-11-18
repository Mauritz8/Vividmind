#include "bits.hpp"

namespace bits {

u_int64_t get(u_int64_t bits, int n) {
  return (bits >> n) & (u_int64_t)1;
}

void set(u_int64_t &bits, int n) { bits |= (u_int64_t)1 << n; }

void unset(u_int64_t &bits, int n) { bits ^= (u_int64_t)1 << n; }

std::optional<int> popLSB(u_int64_t &bits) {
  int i = 0;
  for (int i = 0; i < 64; i++) {
    u_int64_t bit = get(bits, i);
    if (bit == 1) {
      unset(bits, i);
      return i;
    }
  }
  return std::nullopt;
}

std::string to_string(u_int64_t bits) {
  std::string out;
  for (int row = 7; row >= 0; row--) {
    for (int col = 7; col >= 0; col--) {
      u_int64_t bit = get(bits, row * 8 + col);
      out += bit == 1 ? "1" : "0";
    }
    out += " ";
  }
  return out;
}
} // namespace bits
