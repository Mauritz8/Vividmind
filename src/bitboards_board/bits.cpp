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

u_int64_t reverse(u_int64_t b) {
  b = (b & 0x5555555555555555) << 1 | ((b >> 1) & 0x5555555555555555);
  b = (b & 0x3333333333333333) << 2 | ((b >> 2) & 0x3333333333333333);
  b = (b & 0x0f0f0f0f0f0f0f0f) << 4 | ((b >> 4) & 0x0f0f0f0f0f0f0f0f);
  b = (b & 0x00ff00ff00ff00ff) << 8 | ((b >> 8) & 0x00ff00ff00ff00ff);

  return (b << 48) | ((b & 0xffff0000) << 16) | ((b >> 16) & 0xffff0000) |
         (b >> 48);
}

} // namespace bits
