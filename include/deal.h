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

// Description of the deal.
class Deal {
public:
  using Orders = std::array<std::shared_ptr<Order>, 2>;
  using RawOrdersArray = std::array<Order*, 2>;

  Deal(Amount amount, Price price, Microseconds moment,
      Dir aggressor_side, Ticks tsc, std::shared_ptr<Order> order_bid,
      std::shared_ptr<Order> order_ask);

  // Returns a direction of the aggressor order, i.e. of the order which was placed later.
  Dir aggressor_side() const {
    return dir_;
  }

  // Returns a price of the deal.
  Price price() const {
    return price_;
  }

  // Returns a volume of the deal, i.e. lots' volume which were executed in this deal.
  Amount amount() const {
    return amount_;
  }

  // Returns a server time of the deal execution in microseconds.
  Microseconds server_time() const {
    return server_time_;
  }

  // Returns an array of two pointers to orders, which were matched in this deal.
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