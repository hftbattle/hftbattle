#pragma once
#include "base/decimal.h"
#include "base/perf_time.h"
#include "base/pows10.h"
#include "base/string_view.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace hftbattle {

struct DigitTables {
  static const unsigned char rev_3digit_lut[];
  static const unsigned char rev_2digit_lut[];
};

namespace impl {

template <typename T>
constexpr auto has_nonrecursive_begin(int) -> decltype(std::begin(std::declval<T>()), true) {
  return !std::is_same<decltype(*std::begin(std::declval<const T>())), const T&>::value;
}

template <typename T>
constexpr bool has_nonrecursive_begin(...) {
  return false;
}

template <typename T>
struct IsNonrecursiveContainer: std::integral_constant<bool, has_nonrecursive_begin<T>(0)> {
};

}  // namespace impl

// TODO(valentin): Specialize some methods for std::string container (terminating_null, etc).
template<typename Container>
class StringStreamBase {
public:
  StringStreamBase() { init(); }

  explicit StringStreamBase(Container buffer) : buffer_(buffer) { init(); }

  template<typename... Args>
  explicit StringStreamBase(Args&& ... args) : buffer_(std::forward<Args>(args)...) { init(); }

  StringStreamBase(const StringStreamBase&) = delete;
  StringStreamBase& operator=(const StringStreamBase&) = delete;
  StringStreamBase(StringStreamBase&&) = default;
  StringStreamBase& operator=(StringStreamBase&&) = default;

  bool empty() const {
    return buffer_.empty();
  }

  void clear() {
    buffer_.clear();
  }

  void add_terminating_null() {
    if (buffer_.empty() || buffer_.back()) {
      buffer_.push_back(0);
    }
  }

  void remove_terminating_null() {
    if (!buffer_.empty() && !buffer_.back()) {
      buffer_.pop_back();
    }
  }

  const char* c_str() {
    add_terminating_null();
    return buffer_.data();
  }

  std::string std_str() const {
    return view().std_str();
  }

  StringView view() const {
    if (buffer_.empty() || buffer_.back()) {
      return { buffer_.data(), buffer_.size() };
    } else {
      return { buffer_.data(), buffer_.size() - 1 };
    }
  }

  char* data() {
    return buffer_.data();
  }

  const char* data() const {
    return buffer_.data();
  }

  size_t size() const {
    return buffer_.size();
  }

  StringStreamBase& operator<<(int x) { put_integral(x); return *this; }
  StringStreamBase& operator<<(unsigned int x) { put_integral(x); return *this; }
  StringStreamBase& operator<<(long int x) { put_integral(x); return *this; }
  StringStreamBase& operator<<(unsigned long int x) { put_integral(x); return *this; }
  StringStreamBase& operator<<(long long int x) { put_integral(x); return *this; }
  StringStreamBase& operator<<(unsigned long long int x) { put_integral(x); return *this; }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
  StringStreamBase& operator<<(T x) { put_double(x); return *this; }

  StringStreamBase& operator<<(const char* x) { put_chars(x); return *this; }
  StringStreamBase& operator<<(char x) { buffer_.push_back(x); return *this; }
  StringStreamBase& operator<<(Decimal d) { put_decimal(d); return *this;}

  StringStreamBase& operator<<(Seconds x) {
    put_integral(x.count());
    return *this << 's';
  }

  StringStreamBase& operator<<(Milliseconds x) {
    put_integral(x.count());
    return *this << "ms";
  }

  StringStreamBase& operator<<(Microseconds x) {
    put_integral(x.count());
    return *this;
  }

  StringStreamBase& operator<<(Nanoseconds x) {
    *this << x.count() / 1000 << '.';
    put_integral(std::abs(x.count() % 1000), 3);
    return *this;
  }

  StringStreamBase& operator<<(Ticks x) {
    put_integral(x.count());
    return *this;
  }

  template <typename T>
  StringStreamBase& operator<<(const std::atomic<T>& x) {
    return *this << x.load();
  }

  template <typename T, typename N>
  StringStreamBase& operator<<(const std::pair<T, N> & x) {
    return *this << x.first << ": " << x.second;
  }

  template <typename T, typename = std::enable_if_t<impl::IsNonrecursiveContainer<T>::value>>
  StringStreamBase& operator<<(const T& x) {
    put_range(std::begin(x), std::end(x));
    return *this;
  }

  StringStreamBase& operator<<(const std::string& x) {
    put_chars(x.c_str(), x.length());
    return *this;
  }

  StringStreamBase& operator <<(const StringView& x) {
    put_chars(x.data(), x.length());
    return *this;
  }

  template <size_t N>
  StringStreamBase& operator <<(const char(& x)[N]) {
    return *this << StringView(x);
  }

  StringStreamBase& operator <<(std::ostream& (*)(std::ostream&)) {
    return *this << '\n';
  }

  template <typename T>
  void put_integral(T x, int32_t fill_zeroes = 0) {
    put_integral_impl(x, fill_zeroes);
  }

  template <typename T>
  void put_double(T x, int32_t fill_zeroes = 0) {
    if (std::isnan(x)) {
      put_chars("nan");
      return;
    }
    if (!std::isfinite(x)) {
      if (x < 0) {
        put_chars("-inf");
      } else {
        put_chars("inf");
      }
      return;
    }
    if (x < 0) {
      buffer_.push_back('-');
      x = -x;
    }
    T power = static_cast<T>(precision_power_);
    x = static_cast<T>(std::round(x * power) / power);
    auto integral = std::llround(std::trunc(x));
    auto fractional = std::llround((x - std::trunc(x)) * power);
    put_decimal(integral, fractional, fill_zeroes);
  }

  void put_decimal(Decimal d, int32_t fill_zeroes = 0) {
    int64_t multiplied = d.get_numerator();
    if (multiplied < 0) {
      buffer_.push_back('-');
      multiplied = -multiplied;
    }
    int64_t integral = multiplied / Decimal::kMultFactor;
    int64_t fractional = multiplied % Decimal::kMultFactor;
    if (precision_ >= Decimal::kMultPow) {
      fractional *= stored_pow10(static_cast<size_t>(precision_ - Decimal::kMultPow));
    } else {
      fractional = divide_and_round(fractional, stored_pow10(static_cast<size_t>(Decimal::kMultPow - precision_)));
      if (fractional >= precision_power_) {
        fractional -= precision_power_;
        ++integral;
      }
    }
    put_decimal(integral, fractional, fill_zeroes);
  }

private:
  void init() {
    precision_ = 6;
    precision_power_ = 1000000;
  }

  template <typename T>
    void put_integral_impl(T x, int32_t fill_zeroes, bool remove_trailing_zeroes = false) {
      // https://code.google.com/p/strtk/source/browse/trunk/strtk.hpp
      static const size_t buffer_size = ((sizeof(T) < 8) ? 16 : 32);
      unsigned char buffer[buffer_size];
      unsigned char* itr = buffer + buffer_size;
      bool negative = (x < 0);
      typedef typename std::make_unsigned<T>::type UT;
      UT ux = static_cast<UT>((negative) ? -x : x);

      if (ux) {
        while (ux >= static_cast<UT>(100)) {
          itr -= 3;
          UT new_ux = ux / 1000;
          memcpy(itr, &DigitTables::rev_3digit_lut[3 * (ux - (new_ux * 1000))], 3);
          ux = new_ux;
        }

        while (ux >= static_cast<UT>(10)) {
          itr -= 2;
          UT new_ux = ux / 100;
          memcpy(itr, &DigitTables::rev_2digit_lut[2 * (ux - (new_ux * 100))], 2);
          ux = new_ux;
        }

        if (ux) {
          *(--itr) = static_cast<unsigned char>('0' + ux);
        }
        if (negative) {
          *(--itr) = '-';
        }
      } else {
        *(--itr) = '0';
      }

      fill_zeroes = fill_zeroes - static_cast<int>((buffer + buffer_size) - itr);
      if (fill_zeroes > 0) {
        itr -= fill_zeroes;
        memset(itr, '0', static_cast<size_t>(fill_zeroes));
      }
      unsigned char* last = buffer + buffer_size;
      if (remove_trailing_zeroes) {
        --last;
        while (*last == '0') {
          --last;
        }
        ++last;
      }
      put_chars(reinterpret_cast<const char*>(itr), static_cast<size_t>(last - itr));
    }

  void put_decimal(int64_t integral, int64_t fractional, int32_t fill_zeroes = 0) {
    put_integral(integral, fill_zeroes);
    if (fractional != 0) {
      buffer_.push_back('.');
      put_integral_impl(fractional, precision_, true);
    }
  }

  template <typename T>
  T divide_and_round(T a, T b) const {
    return (a + (b / 2)) / b;
  }

  void put_chars(const char* chars, size_t length) {
    buffer_.insert(buffer_.end(), chars, chars + length);
  }

  void put_chars(const char* chars) {
    if (chars) {
      put_chars(chars, strlen(chars));
    }
  }

  template<typename Iterator>
  void put_range(Iterator begin, Iterator end) {
    *this << "[ ";
    bool comma_required = false;
    for (; begin != end; ++begin) {
      if (comma_required) {
        *this << ", ";
      }
      comma_required = true;
      *this << *begin;
    }
    *this << " ]";
  }

  Container buffer_;
  int32_t precision_power_;
  int8_t precision_;
};

using StringStream = StringStreamBase<std::vector<char>>;

template<typename T>
std::string to_std_string(T value) {
  StringStream ss;
  ss << value;
  return ss.std_str();
}

}  // namespace hftbattle
