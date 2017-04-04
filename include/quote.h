#pragma once
#include "base/common_enums.h"
#include "base/constants.h"
#include "base/perf_time.h"

namespace hftbattle {

// A default price to describe an empty level.
// Takes a direction.
// Returns a default price for the quote with given direction.
// Note: if the quote doesn't exist, then it's price is equal to default_quote_price(dir).
inline Price default_quote_price(Dir dir) {
  return dir == BID ? kMinPrice : kMaxPrice;
}

class Order;

// Description of a price level in the order book.
class Quote {
public:
  // Returns a direction of the quote.
  Dir dir() const {
    return dir_;
  }

  // Returns a price of the quote.
  Price price() const {
    return price_;
  }

  // Returns total volume of lots in orders in this quote.
  Amount volume() const {
    return volume_;
  }

  // Returns the latest server time, when the quote has been changed, in microseconds.
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