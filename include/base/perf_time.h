#pragma once
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <ratio>
#include <type_traits>

using namespace std::chrono_literals;
using std::chrono::duration_cast;

namespace hftbattle {

using Nanoseconds = std::chrono::nanoseconds;
using Microseconds = std::chrono::microseconds;
using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;
using Minutes = std::chrono::minutes;
using Hours = std::chrono::hours;

static_assert(std::is_same<int64_t, Nanoseconds::rep>::value, "wrong underlying time type");
static_assert(std::is_same<int64_t, Microseconds::rep>::value, "wrong underlying time type");
static_assert(std::is_same<int64_t, Milliseconds::rep>::value, "wrong underlying time type");
static_assert(std::is_same<int64_t, Seconds::rep>::value, "wrong underlying time type");

class Ticks {
public:
  using base_type = int64_t;

  Ticks() = default;
  explicit constexpr Ticks(base_type count): count_(count) {}

  explicit constexpr operator bool() const { return count_ != 0; }
  constexpr base_type count() const { return count_; }

  static Ticks from_microseconds(Microseconds microseconds) {
    return Ticks(microseconds.count() * get_ticks_in_microsecond());
  }

  static constexpr Ticks zero() {
    return Ticks(0);
  }

  static int64_t get_ticks_in_microsecond();

  Microseconds as_microseconds() const;

private:
  base_type count_;
};

inline bool operator==(Ticks lhs, Ticks rhs) { return lhs.count() == rhs.count(); }
inline bool operator!=(Ticks lhs, Ticks rhs) { return lhs.count() != rhs.count(); }
inline bool operator>(Ticks lhs, Ticks rhs) { return lhs.count() > rhs.count(); }
inline bool operator<(Ticks lhs, Ticks rhs) { return lhs.count() < rhs.count(); }
inline bool operator>=(Ticks lhs, Ticks rhs) { return lhs.count() >= rhs.count(); }
inline bool operator<=(Ticks lhs, Ticks rhs) { return lhs.count() <= rhs.count(); }
inline Ticks operator-(Ticks lhs, Ticks rhs) { return Ticks(lhs.count() - rhs.count()); }
inline Ticks operator+(Ticks lhs, Ticks rhs) { return Ticks(lhs.count() + rhs.count()); }

Ticks rdtsc();

Microseconds microseconds_distance(Ticks t1, Ticks t2);

}  // namespace hftbattle

namespace std {
template<>
class numeric_limits<hftbattle::Ticks> {
public:
  static constexpr hftbattle::Ticks max() {
    return hftbattle::Ticks(numeric_limits<hftbattle::Ticks::base_type>::max());
  }
  static constexpr hftbattle::Ticks min() {
    return hftbattle::Ticks(numeric_limits<hftbattle::Ticks::base_type>::min());
  }
};

template <>
struct hash<hftbattle::Ticks> {
  size_t operator()(hftbattle::Ticks d) const {
    return hash<int64_t>()(d.count());
  }
};

}  // namespace std
