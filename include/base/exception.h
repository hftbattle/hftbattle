#pragma once
#include "base/string_stream.h"
#include <exception>
#include <string>
#include <utility>

namespace hftbattle {

class Exception : public std::exception {
public:
  Exception() {
    what_.add_terminating_null();
  }

  virtual const char* what() const noexcept override {
    return what_.data();
  }

  template <typename T>
  Exception&& operator<<(const T& t) && {
    what_.remove_terminating_null();
    what_ << t;
    what_.add_terminating_null();
    return std::move(*this);
  }

private:
  StringStream what_;
};

}  // namespace hftbattle
