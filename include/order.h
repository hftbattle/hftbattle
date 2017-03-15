#pragma once
#include "base/common_enums.h"
#include "base/constants.h"
#include "base/perf_time.h"
#include "helpers/id_types.h"
#include <algorithm>
#include <cstddef>
#include <limits>
#include <string>

namespace hftbattle {

// docs.order.class
class Order {
public:
  // docs.order.id
  Id id() const {
    return id_;
  }

  // docs.order.dir
  Dir dir() const {
    return dir_;
  }

  // docs.order.price
  Price price() const {
    return price_;
  }

  // docs.order.amount
  Amount amount() const {
    return amount_;
  }

  // docs.order.amount_rest
  Amount amount_rest() const {
    return amount_rest_;
  }

  // docs.order.security_id
  SecurityId security_id() const {
    return security_id_;
  }

  // docs.order.status
  OrderStatus status() const {
    return status_;
  }

  // docs.order.server_time
  Microseconds server_time() const {
    return server_time_;
  }

  virtual ~Order() = default;
  Order& operator=(Order&&) = delete;
  Order(Order&&) = delete;
  Order(const Order&) = delete;
  Order& operator=(const Order&) = delete;

protected:
  Order(SecurityId security_id, Dir dir, Price price, Amount amount,
      Microseconds moment, StrategyId strategy_id, Id ext_id, OrderStatus status = OrderStatus::Active);
  friend class ParticipantStrategy;

  const Id id_;
  Microseconds server_time_;
  const Price price_;
  SecurityId security_id_;
  const Amount amount_;
  const Dir dir_;
  Amount amount_rest_;
  OrderStatus status_;
  StrategyId strategy_id_;
};

}  // namespace hftbattle
