#pragma once
#include "base/decimal.h"
#include "base/exception.h"
#include "base/log.h"
#include "base/string_view.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <utility>

namespace Json {
class Value;
class ValueIterator;
class ValueConstIterator;
}  // namespace Json

namespace hftbattle {
class JsonView;
class JsonValue;
namespace impl {

template <typename T>
std::string to_printable_impl(const T&, ...) {
  return "[not printable]";
}

template <typename T>
decltype(auto) to_printable(const T& t) {
  return to_printable_impl(t, 0);
}

}  // namespace impl

enum class JsonValueType : int8_t {
  Null,
  Int,
  Real,
  Bool,
  String,
  Array,
  Object
};

template <typename IteratorType, typename ReferenceType>
class JsonIteratorBase {
public:
  explicit JsonIteratorBase(const IteratorType&);
  ~JsonIteratorBase();

  JsonIteratorBase(JsonIteratorBase&&);
  JsonIteratorBase(const JsonIteratorBase&);
  JsonIteratorBase& operator=(JsonIteratorBase&&);
  JsonIteratorBase& operator=(const JsonIteratorBase&);

  JsonIteratorBase& operator++();
  JsonIteratorBase operator++(int);
  bool operator==(const JsonIteratorBase&) const;
  bool operator!=(const JsonIteratorBase& other) const { return !(*this == other); }
  ReferenceType operator*() const;

private:
  std::unique_ptr<IteratorType> iter_;
};

using JsonIterator = JsonIteratorBase<Json::ValueIterator, JsonView>;
using JsonConstIterator = JsonIteratorBase<Json::ValueConstIterator, const JsonView>;

class JsonPath {
public:
  struct JsonPathNode {
    std::string key;
    size_t index;
    bool is_map;
  };

  JsonPath() {}
  JsonPath(std::string origin_path, std::vector<JsonPathNode> path):
      origin_path_(std::move(origin_path)), path_(std::move(path)) {}

  void append(std::string key) { path_.push_back({std::move(key), 0, true}); }
  void append(size_t index) { path_.push_back({std::string{}, index, false}); }
  Json::Value* advance(Json::Value* ptr) const;
  const Json::Value* advance(const Json::Value* ptr) const;
  std::string to_string() const;
  JsonPath squashed() const { return {to_string(), {}}; }

private:
  std::string origin_path_;
  std::vector<JsonPathNode> path_;
};

template<typename Container>
class StringStreamBase;

template<typename Container>
StringStreamBase<Container>& operator<<(StringStreamBase<Container>& ss, const JsonPath& path) {
  return ss << path.to_string();
}

class JsonView {
public:
  template <typename I, typename R>
  friend class JsonIteratorBase;

  JsonView& operator=(JsonView&& other);
  JsonView& operator=(const JsonView& other);
  JsonView& operator=(std::string value);
  JsonView& operator=(StringView value) { return *this = value.std_str(); }
  JsonView& operator=(const char* value) { return *this = std::string(value); }
  JsonView& operator=(int value);
  JsonView& operator=(unsigned int value);
  JsonView& operator=(long value);
  JsonView& operator=(unsigned long value);
  JsonView& operator=(long long value);
  JsonView& operator=(unsigned long long value);
  JsonView& operator=(double value);
  JsonView& operator=(bool value);
  JsonView& operator=(std::nullptr_t);
  ~JsonView() = default;

  template <typename T>
  void push_back(T&& element) {
    if (type() != JsonValueType::Null && type() != JsonValueType::Array) {
      *this = nullptr;
    }
    size_t index = size();
    (*this)[index] = std::forward<T>(element);
  }

  JsonValueType type() const;

  size_t size() const;
  bool empty() const;

  JsonIterator begin();
  JsonIterator end();

  JsonConstIterator begin() const;
  JsonConstIterator end() const;

  const JsonView operator[](size_t index) const;
  const JsonView operator[](const std::string& key) const;
  JsonView operator[](size_t index);
  JsonView operator[](const std::string& key);

  // Transforms json-value to a value of given type and returns it.
  // The usage example to get a string — `config["field_in_json"].as<std::string>()`.
  // You are able to set a default return value in brackets (it will be used if there's no such field in config):
  // - `config["field_in_json"].as<std::string>("default_string")`.
  // - `config["time"].as<Microseconds>(10us)`.
  template<typename T>
  T as() const {
    check_defined();
    WARNING_IF(empty()) << "Using empty json value at path: '" << path_ << "'";
    return unchecked_as(TypeTag<T>());
  }

  template<typename T>
  T as(const T& default_value) const {
    if (!defined()) {
      INFO() << "Using default json value: " << impl::to_printable(default_value) <<
          " at path: '" << path_ << "'";
      return default_value;
    }
    WARNING_IF(empty()) << "Using empty json value at path: '" << path_ << "'";
    return unchecked_as(TypeTag<T>());
  }

  std::string to_styled_string() const;
  std::string to_compact_string() const;

  const JsonPath& path() const { return path_; }
  const Json::Value* ptr() const { return path_.advance(static_cast<const Json::Value*>(root_)); }
  Json::Value* ptr() { return path_.advance(root_); }

  bool has_key(const std::string& key) const;

protected:
  template<typename T>
  struct TypeTag {};

  JsonView(JsonPath path, Json::Value* root) : path_(std::move(path)), root_(root) {}

  JsonView(JsonView&& original) = default;
  JsonView(const JsonView& original) = default;

  std::string unchecked_as(TypeTag<std::string>) const;
  StringView unchecked_as(TypeTag<StringView>) const;
  bool unchecked_as(TypeTag<bool>) const;
  int8_t unchecked_as(TypeTag<int8_t>) const;
  uint8_t unchecked_as(TypeTag<uint8_t>) const;
  short unchecked_as(TypeTag<short>) const;
  unsigned short unchecked_as(TypeTag<unsigned short>) const;
  int unchecked_as(TypeTag<int>) const;
  unsigned int unchecked_as(TypeTag<unsigned int>) const;
  long unchecked_as(TypeTag<long>) const;
  unsigned long unchecked_as(TypeTag<unsigned long>) const;
  long long unchecked_as(TypeTag<long long>) const;
  unsigned long long unchecked_as(TypeTag<unsigned long long>) const;
  Decimal unchecked_as(TypeTag<Decimal>) const;
  double unchecked_as(TypeTag<double>) const;
  JsonValue unchecked_as(TypeTag<JsonValue>) const;

  template<typename T, typename R>
  auto unchecked_as(TypeTag<std::chrono::duration<T, R>>) const {
    return std::chrono::duration<T, R>(unchecked_as(TypeTag<T>()));
  }

  template<typename T>
  std::vector<T> unchecked_as(TypeTag<std::vector<T>>) const {
    CHECK(empty() || type() == JsonValueType::Array,
        "Can't cast json to vector: '" << path() << "'");
    std::vector<T> result;
    result.reserve(size());
    std::transform(
      begin(),
      end(),
      std::back_inserter(result),
      [](const JsonView& view) { return view.as<T>(); });
    return result;
  }

  bool defined() const;
  void check_defined() const;

  JsonPath path_;
  Json::Value* const root_;
};

// JSON data format is described here: <https://en.wikipedia.org/wiki/JSON>.
// In our implementation you can edit your strategy configuration in .json file.
// To get the value from the Json use "as" method.
// This class is only used in C++, in **Python** standard dict is used.
class JsonValue : private std::unique_ptr<Json::Value>, public JsonView {
public:
  using iterator = JsonIterator;
  using const_iterator = JsonConstIterator;

  JsonValue();

  ~JsonValue();

  explicit JsonValue(JsonValueType value_type);
  explicit JsonValue(const Json::Value& value);

  JsonValue(const JsonValue& value);
  /* implicit */ JsonValue(const JsonView& view);  // NOLINT(runtime/explicit)

  JsonValue& operator=(const JsonValue& value);
  JsonValue& operator=(const JsonView& view);

  Json::Value* get_raw() { return get(); }
};

}  // namespace hftbattle