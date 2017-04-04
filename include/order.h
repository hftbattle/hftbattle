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

// Description of the market order.
class Order {
public:
  // Returns the unique numeric identifier of an order, which was received during a simulation.
  // It can be used to aggregate some information about the order.
  Id id() const {
    return id_;
  }

  // Returns a direction of the order.
  Dir dir() const {
    return dir_;
  }

  // Returns a price of the order.
  Price price() const {
    return price_;
  }

  // Returns an initial volume of the order (the amount of lots).
  Amount amount() const {
    return amount_;
  }

  // Returns current volume of the order (the amount of lots which have not been matched with other orders yet).
  Amount amount_rest() const {
    return amount_rest_;
  }

  // Returns an enum class value — order's status.
  // Possible statuses are: Adding, Active, Deleting, Deleted.
  // Please read more in the OrderStatus class description: <https://docs.hftbattle.com/en/api/CommonEnums.html#orderstatus>.
  OrderStatus status() const {
    return status_;
  }

  // Returns the server time, when the order was placed, in microseconds.
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