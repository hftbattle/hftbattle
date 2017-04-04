#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>

namespace hftbattle {

template <typename T>
using IsConvertibleToDecimal = std::integral_constant<
    bool,
    std::is_integral<T>::value || std::is_same<T, double>::value
>;

// Decimal class is used for performing high precision calculations and for price representation (for convenience, it has Price alias).
// You can also get double using get_double() method.
// Note: in C++ you can't print Decimal using cout/cerr/printf, however, you can use our output streams.
// In Python you can use print.
class Decimal {
private:
  struct FromNumeratorTag{};
public:
  static const int64_t kMultPow = 7;
  static const int64_t kMaxPow = 18;
  static const int64_t kMultFactor = 10'000'000;  // 10^7
  constexpr Decimal() : number_(0) {}

  template<typename T, class = std::enable_if_t<std::is_integral<T>::value>>
  constexpr Decimal(T value): number_(static_cast<int64_t>(value) * kMultFactor) {
  }

  Decimal(int64_t mantissa, int16_t exponent);

  constexpr Decimal(double r) :
    number_(static_cast<int64_t>(kMultFactor * r + (r > 0 ? 0.5 : -0.5))) {
  }

  Decimal& operator+=(Decimal rhs);
  Decimal& operator-=(Decimal rhs);

  Decimal& operator*=(Decimal rhs);
  Decimal& operator/=(Decimal rhs);

  template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
  Decimal& operator*=(T rhs);

  template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
  Decimal& operator/=(T rhs);

  constexpr double get_double() const {
    return static_cast<double>(number_) / static_cast<double>(kMultFactor);
  }

  int32_t get_int() const {
    if (number_ >= 0) {
      return static_cast<int32_t>((number_ + kMultFactor / 2) / kMultFactor);
    } else {
      return -static_cast<int32_t>((-number_ + kMultFactor / 2) / kMultFactor);
    }
  }

  constexpr int64_t get_numerator() const {
    return number_;
  }

  constexpr static Decimal from_numerator(int64_t r) {
    return Decimal(r, FromNumeratorTag());
  }

  int64_t integer_division(Decimal div) const;

  friend constexpr Decimal operator-(Decimal rhs);

private:
  constexpr Decimal(int64_t number, FromNumeratorTag): number_(number) {}
  int64_t number_;
};

Decimal operator* (Decimal lhs, double rhs);
Decimal operator/ (Decimal lhs, double rhs);
Decimal operator* (double lhs, Decimal rhs);
Decimal operator/ (double lhs, Decimal rhs);
Decimal operator* (Decimal lhs, Decimal rhs);
Decimal operator/ (Decimal lhs, Decimal rhs);

inline Decimal& Decimal::operator/=(Decimal rhs) {
  return *this = *this / rhs;
}

inline Decimal& Decimal::operator*=(Decimal rhs) {
  return *this = *this * rhs;
}

template<typename T, class>
inline Decimal& Decimal::operator/=(T rhs) {
  return *this = *this / rhs;
}

template<typename T, class>
inline Decimal& Decimal::operator*=(T rhs) {
  return *this = *this * rhs;
}

inline Decimal& Decimal::operator-=(Decimal rhs) {
  number_ -= rhs.number_;
  return *this;
}

inline Decimal& Decimal::operator+=(Decimal rhs) {
  number_ += rhs.number_;
  return *this;
}

inline Decimal abs(Decimal d) {
  return d.get_numerator() >= 0 ? d: -d;
}

constexpr inline Decimal operator "" _dc(long double ld) {
  return Decimal(static_cast<double>(ld));
}

inline Decimal operator "" _dc(unsigned long long ull) {
  return Decimal::from_numerator(static_cast<int64_t>(ull) * Decimal::kMultFactor);
}

inline constexpr Decimal operator-(Decimal rhs) {
  return Decimal::from_numerator(-rhs.get_numerator());
}

//==============================
// lhs: Decimal rhs: Decimal
//==============================
inline constexpr bool operator==(Decimal lhs, Decimal rhs) {
  return lhs.get_numerator() == rhs.get_numerator();
}

inline constexpr bool operator!=(Decimal lhs, Decimal rhs) {
  return lhs.get_numerator() != rhs.get_numerator();
}

inline constexpr bool operator<(Decimal lhs, Decimal rhs) {
  return lhs.get_numerator() < rhs.get_numerator();
}

inline constexpr bool operator<=(Decimal lhs, Decimal rhs) {
  return lhs.get_numerator() <= rhs.get_numerator();
}

inline constexpr bool operator>(Decimal lhs, Decimal rhs) {
  return lhs.get_numerator() > rhs.get_numerator();
}

inline constexpr bool operator>=(Decimal lhs, Decimal rhs) {
  return lhs.get_numerator() >= rhs.get_numerator();
}

inline constexpr Decimal operator+(Decimal lhs, Decimal rhs) {
  return Decimal::from_numerator(lhs.get_numerator() + rhs.get_numerator());
}

inline constexpr Decimal operator-(Decimal lhs, Decimal rhs) {
  return Decimal::from_numerator(lhs.get_numerator() - rhs.get_numerator());
}

//==============================
// lhs: Decimal rhs: ConvertibleToDecimal
//==============================

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr bool operator==(Decimal lhs, T rhs) {
  return lhs == Decimal(rhs);
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr bool operator!=(Decimal lhs, T rhs) {
  return lhs != Decimal(rhs);
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr bool operator<(Decimal lhs, T rhs) {
  return lhs < Decimal(rhs);
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr bool operator<=(Decimal lhs, T rhs) {
  return lhs <= Decimal(rhs);
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr bool operator>(Decimal lhs, T rhs) {
  return lhs > Decimal(rhs);
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr bool operator>=(Decimal lhs, T rhs) {
  return lhs >= Decimal(rhs);
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr Decimal operator+(Decimal lhs, T rhs) {
  return lhs + Decimal(rhs);
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr Decimal operator-(Decimal lhs, T rhs) {
  return lhs - Decimal(rhs);
}

//==============================
// lhs: ConvertibleToDecimal rhs: Decimal
//==============================

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr bool operator==(T lhs, Decimal rhs) {
  return Decimal(lhs) == rhs;
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr bool operator!=(T lhs, Decimal rhs) {
  return Decimal(lhs) != rhs;
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr bool operator<(T lhs, Decimal rhs) {
  return Decimal(lhs) < rhs;
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr bool operator<=(T lhs, Decimal rhs) {
  return Decimal(lhs) <= rhs;
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr bool operator>(T lhs, Decimal rhs) {
  return Decimal(lhs) > rhs;
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr bool operator>=(T lhs, Decimal rhs) {
  return Decimal(lhs) >= rhs;
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr Decimal operator+(T lhs, Decimal rhs) {
  return Decimal(lhs) + rhs;
}

template<typename T, class = std::enable_if_t<IsConvertibleToDecimal<T>::value>>
inline constexpr Decimal operator-(T lhs, Decimal rhs) {
  return Decimal(lhs) - rhs;
}

//==============================
// lhs: Decimal rhs: integral
//==============================

template<typename T, class = std::enable_if_t<std::is_integral<T>::value>>
inline constexpr Decimal operator*(Decimal lhs, T rhs) {
  return Decimal::from_numerator(lhs.get_numerator() * static_cast<int64_t>(rhs));
}

template<typename T, class = std::enable_if_t<std::is_integral<T>::value>>
inline Decimal operator/(Decimal lhs, T rhs) {
  auto numerator = lhs.get_numerator();
  int64_t divisor = static_cast<int64_t>(rhs);
  if (divisor < 0) {
    divisor = -divisor;
    numerator = -numerator;
  }
  if (numerator >= 0) {
    return Decimal::from_numerator((numerator + divisor / 2) / divisor);
  } else {  // unlikely
    return Decimal::from_numerator(-((-numerator + divisor / 2) / divisor));
  }
}

//==============================
// lhs: integral rhs: Decimal
//==============================

template<typename T, class = std::enable_if_t<std::is_integral<T>::value>>
inline constexpr Decimal operator*(T lhs, Decimal rhs) {
  return Decimal::from_numerator(static_cast<int64_t>(lhs) * rhs.get_numerator());
}

}  // namespace hftbattle

namespace std {
template <>
struct hash<hftbattle::Decimal> {
  size_t operator()(hftbattle::Decimal d) const {
    return hash<int64_t>()(d.get_numerator());
  }
};

}  // namespace std