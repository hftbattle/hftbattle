#pragma once
#include "base/constants.h"
#include <array>
#include <map>
#include <vector>

namespace hftbattle {

class Order;

// Description of your current orders.
// Only orders with Adding or Active status are taken into account.
// In deleting_amount_by_dir method orders with Deleting status are also counted.
// The snapshot is updated before every update in strategy.
// It remains unchanged during single update processing.
class SecurityOrdersSnapshot {
public:
  using Orders = std::vector<Order*>;
  using OrdersMap = std::map<Price, Orders>;

  SecurityOrdersSnapshot();

  // Takes a direction and a price.
  // Returns total volume of the orders with given price and direction.
  Amount volume(Dir dir, Price price) const;

  // Takes a direction.
  // Returns a number of your current orders with given direction.
  size_t size_by_dir(Dir dir) const {
    return orders_by_dir_[dir].size();
  }

  // Takes a direction.
  // Returns a number of your active orders with given direction, i.e. orders with Active status.
  size_t active_orders_count(Dir dir) const;

  // Takes a direction.
  // Returns total volume of your active orders with given direction, i.e. orders with Active status.
  Amount active_orders_volume(Dir dir) const;

  // Takes a direction.
  // Returns a vector of pointers to your orders with given direction.
  const Orders& orders_by_dir(Dir dir) const {
    return orders_by_dir_[dir];
  }

  // Takes a direction.
  // Returns a map from price to vector of pointers to your orders with given direction.
  // Note: the map is always ordered by price in ascending order.
  OrdersMap orders_by_dir_as_map(Dir dir) const;

  // Takes a direction.
  // Returns a total volume of your orders with given direction, which had been sent to deletion, but have not been deleted (the ones with Deleting status) yet.
  Amount deleting_amount_by_dir(Dir dir) const {
    return deleting_amount_[dir];
  }

private:
  friend class OrdersStorage;
  friend class ConstraintsChecker;

  void clear();

  Amount implied_amount() const {
    return implied_amount_;
  }

  std::array<Orders, 2> orders_by_dir_;
  std::array<Amount, 2> deleting_amount_;
  Amount implied_amount_;
};

}  // namespace hftbattle