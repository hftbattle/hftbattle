#pragma once
#include "base/constants.h"
#include <array>
#include <map>
#include <vector>

namespace hftbattle {

class Order;

// docs.security_orders_snapshot.class
class SecurityOrdersSnapshot {
public:
  using Orders = std::vector<Order*>;
  using OrdersMap = std::map<Price, Orders>;

  SecurityOrdersSnapshot();

  // docs.security_orders_snapshot.volume
  Amount volume(Dir dir, Price price) const;

  // docs.security_orders_snapshot.size_by_dir
  size_t size_by_dir(Dir dir) const {
    return orders_by_dir_[dir].size();
  }

  // docs.security_orders_snapshot.active_orders_count
  size_t active_orders_count(Dir dir) const;

  // docs.security_orders_snapshot.active_orders_volume
  Amount active_orders_volume(Dir dir) const;

  // docs.security_orders_snapshot.orders_by_dir
  const Orders& orders_by_dir(Dir dir) const {
    return orders_by_dir_[dir];
  }

  // docs.security_orders_snapshot.orders_by_dir_as_map
  OrdersMap orders_by_dir_as_map(Dir dir) const;

  // docs.security_orders_snapshot.deleting_amount_by_dir
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
