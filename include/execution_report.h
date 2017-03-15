#pragma once
#include "base/constants.h"

namespace hftbattle {

class Order;

// docs.execution_report.class
class ExecutionReport {
public:
  // docs.execution_report.order
  Order* order() const {
    return order_;
  }

  // docs.execution_report.price
  Price price() const {
    return price_;
  }

  // docs.execution_report.amount
  Amount amount() const {
    return amount_;
  }

  // docs.execution_report.dir
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
