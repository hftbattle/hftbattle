#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace hftbattle {
namespace impl {

constexpr int64_t int_pow10(size_t n) {
  return n == 0 ? 1 : 10 * int_pow10(n - 1);
}

constexpr std::array<int64_t, 19> kPowsOf10 = {
  int_pow10(0),
  int_pow10(1),
  int_pow10(2),
  int_pow10(3),
  int_pow10(4),
  int_pow10(5),
  int_pow10(6),
  int_pow10(7),
  int_pow10(8),
  int_pow10(9),
  int_pow10(10),
  int_pow10(11),
  int_pow10(12),
  int_pow10(13),
  int_pow10(14),
  int_pow10(15),
  int_pow10(16),
  int_pow10(17),
  int_pow10(18)
};
static_assert(kPowsOf10[2] == 100, "loool");

}  // impl

inline constexpr int64_t stored_pow10(std::size_t n) {
  return impl::kPowsOf10[n];
}

}  // namespace hftbattle
