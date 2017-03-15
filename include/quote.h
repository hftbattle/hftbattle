#pragma once
#include "base/common_enums.h"
#include "base/constants.h"
#include "base/perf_time.h"

namespace hftbattle {

// docs.default_quote_price.class
inline Price default_quote_price(Dir dir) {
  return dir == BID ? kMinPrice : kMaxPrice;
}

class Order;

// docs.quote.class
class Quote {
public:
  // docs.quote.dir
  Dir dir() const {
    return dir_;
  }

  // docs.quote.price
  Price price() const {
    return price_;
  }

  // docs.quote.volume
  Amount volume() const {
    return volume_;
  }

  // docs.quote.server_time
  Microseconds server_time() const {
    return last_moment_;
  }

  virtual ~Quote() = default;

protected:
  Quote(Dir dir, Price price, Microseconds last_moment, Ticks last_tsc, Amount volume = 0);

  Dir dir_;
  Price price_;
  Amount volume_;
  Microseconds last_moment_;
  Ticks last_tsc_;
};

}  // namespace hftbattle
