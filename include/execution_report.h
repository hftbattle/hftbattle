#pragma once
#include "base/constants.h"

namespace hftbattle {

class Order;

// Report on a deal with your order.
class ExecutionReport {
public:
  // Returns a pointer to your order, that was matched in the deal.
  Order* order() const {
    return order_;
  }

  // Returns a price of the deal.
  Price price() const {
    return price_;
  }

  // Returns a volume of the deal.
  Amount amount() const {
    return amount_;
  }

  // Returns a direction of your executed order.
  Dir dir() const;

  virtual ~ExecutionReport() = default;

protected:
  ExecutionReport(
      Microseconds server_time,
      Order* order,
      Price price,
      Amount amount
  );

  Microseconds server_time_;
  Order* order_;
  const Price price_;
  const Amount amount_;
};

}  // namespace hftbattle