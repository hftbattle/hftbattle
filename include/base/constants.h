#pragma once
#include "base/common_enums.h"
#include "base/decimal.h"
#include <chrono>
#include <string>

namespace hftbattle {

// An alias for int32_t for better code readability.
// This alias is only used in C++.
using Amount = int32_t;

// An alias for Decimal class, you can read about Decimal class here: <https://docs.hftbattle.com/en/api/Other.html#decimal>.
using Price = Decimal;

// A numeric identifier, an alias for uint64_t.
// This alias is only used in C++.
using Id = uint64_t;

// An alias for std::chrono::microseconds.
// Read more information here: <http://en.cppreference.com/w/cpp/chrono/duration>.
// This class is only used in C++, in **Python** standard int type is used.
using Microseconds = std::chrono::microseconds;

static constexpr Price kMinPrice(0);
static constexpr Price kMaxPrice(100000000);

static constexpr Decimal kMinStopLossResult = Decimal(-100000.0);
static constexpr Amount kMaximumMaxExecutedAmount = 100;
static constexpr size_t kMaxViewerMomentsCount = 50;
static constexpr size_t kMaxOrdersCountPlain = 2000000;
static constexpr size_t kMaxOrdersCountDuel = 4000000;

namespace simulator {

const std::string kDefaultLogLevel = "fatal";

class Security;
using SecurityId = const Security*;

}  // namespace simulator

using simulator::SecurityId;

}  // namespace hftbattle