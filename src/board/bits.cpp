#include "bits.hpp"
#include <strings.h>

namespace bits {

uint64_t get(uint64_t bits, int n) { return (bits >> n) & (uint64_t)1; }

void set(uint64_t &bits, int n) { bits |= (uint64_t)1 << n; }

void unset(uint64_t &bits, int n) { bits ^= (uint64_t)1 << n; }

// https://www.chessprogramming.org/BitScan#DeBruijnMultiplation
const int index64[64] = {
  0,  1,  48,  2, 57, 49, 28,  3,
  61, 58, 50, 42, 38, 29, 17,  4,
  62, 55, 59, 36, 53, 51, 43, 22,
  45, 39, 33, 30, 24, 18, 12,  5,
  63, 47, 56, 27, 60, 41, 37, 16,
  54, 35, 52, 21, 44, 32, 23, 11,
  46, 26, 40, 15, 34, 20, 31, 10,
  25, 14, 19,  9, 13,  8,  7,  6
};
int first_set_bit(uint64_t bits) {
    bits &= -bits;
    return index64[(bits * 0x03F79D71B4CB0A89) >> 58];
}

int popLSB(uint64_t &bits) {
  const int i = first_set_bit(bits);
  unset(bits, i);
  return i;
}

std::string to_string(uint64_t bits) {
  std::string out;
  for (int row = 7; row >= 0; row--) {
    for (int col = 7; col >= 0; col--) {
      uint64_t bit = get(bits, row * 8 + col);
      out += bit == 1 ? "1" : "0";
    }
    out += " ";
  }
  return out;
}

// TODO: Using a library function should be faster
// It would depend on the operating system,
// bswap on linux and __byteswap_uint64 on windows
// https://man7.org/linux/man-pages/man3/bswap.3.html
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/byteswap-uint64-byteswap-ulong-byteswap-ushort
// I tried it but it seems like it's not doing the same thing, I need to
// investigate
uint64_t reverse(uint64_t b) {
  b = (b & 0x5555555555555555) << 1 | ((b >> 1) & 0x5555555555555555);
  b = (b & 0x3333333333333333) << 2 | ((b >> 2) & 0x3333333333333333);
  b = (b & 0x0f0f0f0f0f0f0f0f) << 4 | ((b >> 4) & 0x0f0f0f0f0f0f0f0f);
  b = (b & 0x00ff00ff00ff00ff) << 8 | ((b >> 8) & 0x00ff00ff00ff00ff);

  return (b << 48) | ((b & 0xffff0000) << 16) | ((b >> 16) & 0xffff0000) |
         (b >> 48);
}

// https://www.chessprogramming.org/Population_Count#Lookup
const uint8_t lookup_table[256] = {
  #define B2(n)  n,      n+1,      n+1,      n+2
  #define B4(n)  B2(n),  B2(n+1),  B2(n+1),  B2(n+2)
  #define B6(n)  B4(n),  B4(n+1),  B4(n+1),  B4(n+2)
  B6(0), B6(1), B6(1), B6(2)
};
int nr_bits_set(uint64_t bits) {
  uint64_t count = 0;
  for (int i = 0; i < 8; i++) {
    count += lookup_table[bits & 0xFF];
    bits >>= 8;
  }
  return count;
}

} // namespace bits
