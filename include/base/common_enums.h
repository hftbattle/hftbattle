#pragma once
#include <cstdint>

namespace hftbattle {

namespace dir_name_helper {
static constexpr char kBidName[] = "BID";
static constexpr char kAskName[] = "ASK";
}  // namespace dir_name_helper

// Enum class describing a side which to draw Y-axis on (left or right).
// - Left — on the left side.
// - Right — on the right side.
enum class ChartYAxisType : int8_t {
  Left,
  Right
};

// The enum class describing possible order statuses:
// - Adding — the order has been created but has not been placed yet.
//   This happens because of the delay between order addition request and order appearance on the market.
//   This delay is called round-trip.
// - Active — the order is placed.
// - Deleting — there had been a request to delete the order, but it is has not been deleted yet.
//   This happens because of the delay between order deletion request and order deletion on the market.
// - Deleted — the order was deleted.
enum class OrderStatus : uint8_t {
  Adding,
  Active,
  Deleting,
  Deleted
};

// The enum describing a direction of deals, orders and quotes.
// - BID, BUY, 0 means purchase.
// - ASK, SELL, 1 means selling.
enum Dir : uint8_t {
  BUY = 0,
  BID = 0,
  SELL = 1,
  ASK = 1,
  UNKNOWN = 3
};

// Takes a direction.
// Returns the opposite direction.
inline Dir opposite_dir(Dir dir) {
  return static_cast<Dir>(1 - dir);
}

// Takes a direction.
// Returns a direction sign (1 for BID and -1 for ASK).
inline int32_t dir_sign(Dir dir) {
  return static_cast<int8_t>(dir) * -2 + 1;
}

inline const char* dir_name(Dir dir) {
  return dir == BID ? dir_name_helper::kBidName : dir_name_helper::kAskName;
}

}  // namespace hftbattle