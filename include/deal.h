#pragma once
#include "base/common_enums.h"
#include "base/constants.h"
#include "base/perf_time.h"
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace hftbattle {

class Order;

// docs.deal.class
class Deal {
public:
  using Orders = std::array<std::shared_ptr<Order>, 2>;
  using RawOrdersArray = std::array<Order*, 2>;

  Deal(Amount amount, Price price, Microseconds moment,
      Dir aggressor_side, Ticks tsc, std::shared_ptr<Order> order_bid,
      std::shared_ptr<Order> order_ask);

  // docs.deal.aggressor_side
  Dir aggressor_side() const {
    return dir_;
  }

  // docs.deal.price
  Price price() const {
    return price_;
  }

  // docs.deal.amount
  Amount amount() const {
    return amount_;
  }

  // docs.deal.server_time
  Microseconds server_time() const {
    return server_time_;
  }

  // docs.deal.orders
  const RawOrdersArray orders() const;

  virtual ~Deal() = default;

protected:
  Amount amount_;
  Price price_;
  Dir dir_;
  Microseconds server_time_;
  Ticks origin_time_;
  Orders orders_;
};

}  // namespace hftbattle
