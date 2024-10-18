#include "bits.hpp"

namespace bits {

u_int64_t get(u_int64_t bits, int n) {
  return (bits >> n) & (u_int64_t)1;
}

void set(u_int64_t &bits, int n) { bits |= (u_int64_t)1 << n; }

void unset(u_int64_t &bits, int n) { bits &= ~((u_int64_t)1 << n); }

std::optional<int> pop(u_int64_t &bits) {
  int i = 0;
  u_int64_t bit;
  do {
    bit = get(bits, i);
    if (bit == 1) {
      unset(bits, i);
      return i;
    }
    i++;
  } while (i < 64);
  return std::nullopt;
}

std::string to_string(u_int64_t bits) {
  std::string out;
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      u_int64_t bit = get(bits, row * 8 + col);
      out += bit == 1 ? "1" : "0";
    }
    out += " ";
  }
  return out;
}
} // namespace bits
