#pragma once
#include "base/exception.h"
#include "base/perf_time.h"
#include "base/string_stream.h"
#include "base/string_view.h"
#include <cstddef>
#include <memory>
#include <string>

namespace hftbattle {

static constexpr char kAnonymousLoggerBackendName[] = "";

class Logger;

typedef Logger* LoggerId;

// docs.log.main_info.class

// docs.log.log_levels.class
enum class LogLevel: int8_t {
  Debug,
  Info,
  Warning,
  Error,
  Screen,
  Fatal
};

LogLevel log_level_from_str(std::string level);

class LogMessage {
public:
  LogMessage(LoggerId logger, LogLevel level) : logger_(logger), level_(level) { }

  LogMessage(const LogMessage&) = delete;
  LogMessage& operator=(const LogMessage&) = delete;
  LogMessage(LogMessage&&) = default;
  LogMessage& operator=(LogMessage&&) = default;

  ~LogMessage();

  LoggerId logger() const { return logger_; }
  LogLevel level() const { return level_; }
  StringView view() const { return StringView(text_.data(), text_.size()); }
  bool empty() const { return text_.empty(); }

  void reset() { text_.clear(); }

  template<typename T>
  LogMessage& operator<<(const T& value) {
    text_ << value;
    return *this;
  }

private:
  LoggerId logger_;
  LogLevel level_;
  StringStream text_;
};

class LoggerBackend;
class TimeBackend;

class Logger {
public:
  static constexpr size_t kMaxLoggerNameLength = 95;

  Logger(const std::string& name, const std::string& backend_name);

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  const std::string& name() const {
    return name_;
  }

  LoggerId id() {
    return this;
  }

  void set_min_level(LogLevel min_level);

  LogLevel min_level() const;

  const LoggerBackend* backend() const;

  LoggerBackend* mutable_backend();

  void forward_logs_to_file(const std::string& filename);
  void set_time_backend(std::unique_ptr<TimeBackend>&& time_backend);

  void flush();

  static LoggerId get_logger_id(const std::string& name, const std::string& logger_backend_name);
  static std::string replace_logger_name_invalid_chars(const std::string& name, char replacer);
  static LoggerId anonymous();

private:
  std::string name_;
  std::shared_ptr<LoggerBackend> backend_;
};

enum class TerminalColor : int8_t {
  Reset,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan
};

const StringView get_terminal_color_code(TerminalColor color);

struct SourceLocation {
    SourceLocation(const char* filename, int line_number, const char* function_name)
      : filename(filename), line_number(line_number), function_name(function_name) {}
    const std::string filename;
    const int line_number;
    const std::string function_name;
};

template<typename T>
StringStreamBase<T>& operator<<(StringStreamBase<T>& stream, const SourceLocation& source_location) {
  return stream << "@ " << source_location.filename <<
    " :" << source_location.line_number <<
    ":" << source_location.function_name << ":";
}

inline LoggerId getCurrentLoggerId() {
  return Logger::anonymous();
}

class LoggableComponent {
public:
  explicit LoggableComponent(const std::string& logger_name,
      const std::string& logger_backend_name = kAnonymousLoggerBackendName);

  const std::string& logger_name() const {
    return getCurrentLoggerId()->name();
  }

protected:
  LoggerId getCurrentLoggerId() const {
    return kCurrentLoggerId;
  }

  const LoggerId kCurrentLoggerId;
};

#define PRIVATE_LOG(logger, level) \
  for (bool _once = true; _once && level >= logger->min_level(); _once = false) \
    hftbattle::LogMessage(logger, level)

#define PRIVATE_LOG_IF(logger, level, condition) \
  for (bool _once = true; _once && level >= logger->min_level() && (condition); _once = false) \
    hftbattle::LogMessage(logger, level)

#define FATAL_WITH_FUNCTION_NAME(function_name) throw hftbattle::Exception() << \
  hftbattle::SourceLocation(__FILE__, __LINE__, function_name) << \
  "[" << getCurrentLoggerId()->name() << "] "

#define FATAL() FATAL_WITH_FUNCTION_NAME(__FUNCTION__)

#ifdef __CIDR_clang__has_attribute_noreturn
#define LAMBDA_ATTRIBUTE(...)
#else
#define LAMBDA_ATTRIBUTE(...) __attribute__(__VA_ARGS__)
#endif

#define CHECK2(condition, message) \
  do { \
    if (!(condition)) { \
      auto _function = __FUNCTION__; \
      [&]() LAMBDA_ATTRIBUTE((noinline, cold)) { \
        FATAL_WITH_FUNCTION_NAME(_function) << "Check failed: '" << (#condition) << "' " << message; \
      }(); \
    } \
  } while (false)

#define CHECK1(condition) \
  do { \
    if (!(condition)) { \
      auto _function = __FUNCTION__; \
      [&]() LAMBDA_ATTRIBUTE((noinline, cold)) { \
        FATAL_WITH_FUNCTION_NAME(_function) << "Check failed: '" << (#condition) << "'"; \
      }(); \
    } \
  } while (false)

#define GET_CHECK(_1, _2, NAME, ...) NAME
#define CHECK(...) GET_CHECK(__VA_ARGS__, CHECK2, CHECK1, CHECK0)(__VA_ARGS__)

#define DEBUG() PRIVATE_LOG(getCurrentLoggerId(), hftbattle::LogLevel::Debug)
#define INFO() PRIVATE_LOG(getCurrentLoggerId(), hftbattle::LogLevel::Info)
#define WARNING() PRIVATE_LOG(getCurrentLoggerId(), hftbattle::LogLevel::Warning)
#define SCREEN() PRIVATE_LOG(getCurrentLoggerId(), hftbattle::LogLevel::Screen)
#define ERROR() PRIVATE_LOG(getCurrentLoggerId(), hftbattle::LogLevel::Error)

#define INFO_IF(condition) PRIVATE_LOG_IF(getCurrentLoggerId(), hftbattle::LogLevel::Info, condition)
#define WARNING_IF(condition) PRIVATE_LOG_IF(getCurrentLoggerId(), hftbattle::LogLevel::Warning, condition)
#define ERROR_IF(condition) PRIVATE_LOG_IF(getCurrentLoggerId(), hftbattle::LogLevel::Error, condition)

}  // namespace hftbattle
