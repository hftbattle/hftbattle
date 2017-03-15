#pragma once
#include <cstdint>

namespace hftbattle {

namespace dir_name_helper {
static constexpr char kBidName[] = "BID";
static constexpr char kAskName[] = "ASK";
}  // namespace dir_name_helper

// docs.common_enums.chart_y_axis_type.class
enum class ChartYAxisType : int8_t {
  Left,
  Right
};

// docs.common_enums.order_status.class
enum class OrderStatus : uint8_t {
  Adding,
  Active,
  Deleting,
  Deleted
};

// docs.common_enums.dir.class
enum Dir : uint8_t {
  BUY = 0,
  BID = 0,
  SELL = 1,
  ASK = 1,
  UNKNOWN = 3
};

// docs.common_enums.opposite_dir.class
inline Dir opposite_dir(Dir dir) {
  return static_cast<Dir>(1 - dir);
}

// docs.common_enums.dir_sign.class
inline int32_t dir_sign(Dir dir) {
  return static_cast<int8_t>(dir) * -2 + 1;
}

inline const char* dir_name(Dir dir) {
  return dir == BID ? dir_name_helper::kBidName : dir_name_helper::kAskName;
}

}  // namespace hftbattle
