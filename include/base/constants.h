#pragma once
#include "base/common_enums.h"
#include "base/decimal.h"
#include <chrono>
#include <string>

namespace hftbattle {

// docs.constants.Amount.class
using Amount = int32_t;

// docs.constants.Price.class
using Price = Decimal;

// docs.constants.Id.class
using Id = uint64_t;

// docs.constants.Microseconds.class
using Microseconds = std::chrono::microseconds;

static constexpr Price kMinPrice(0);
static constexpr Price kMaxPrice(100000000);

static constexpr Decimal kMinStopLossResult = Decimal(-100000.0);
static constexpr Amount kMaximumMaxExecutedAmount = 100;
static constexpr size_t kMaxViewerMomentsCount = 50;

namespace simulator {

const std::string kDefaultLogLevel = "fatal";

class Security;
using SecurityId = const Security*;

}  // namespace simulator

using simulator::SecurityId;

}  // namespace hftbattle
