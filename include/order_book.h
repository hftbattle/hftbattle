#pragma once
#include "quote.h"
#include "quotes_holder.h"
#include "security_orders_snapshot.h"
#include "base/common_enums.h"
#include "base/constants.h"
#include "base/perf_time.h"
#include <array>

namespace hftbattle {

// This class aggregates all orders for a specific instrument.
// Note: indices are counted from 0, beginning from the best price, i.e. in descending order for BID (Buy) and in ascending order for ASK (Sell).
// **Only non-empty quotes** are taken into account.
// So if a quote has index 3, that doesn't mean that it's price differs in exactly 3 minimum steps from the best price.
class OrderBook {
public:
  // Takes a direction and an index.
  // Returns a quote with given direction and index.
  const Quote& quote_by_index(Dir dir, size_t index) const;

  // Takes a direction and an index.
  // Returns a price of the quote with given direction and index.
  // Note: if the quote doesn't exist, corresponding default_quote_price is returned.
  Price price_by_index(Dir dir, size_t index) const {
    return quote_by_index(dir, index).price();
  }

  // Takes a direction and an index.
  // Returns a volume of the quote with given direction and index.
  Amount volume_by_index(Dir dir, size_t index) const {
    return quote_by_index(dir, index).volume();
  }

  // Takes a direction and a price.
  // Returns a quote with given direction and price.
  const Quote& quote_by_price(Dir dir, Price price) const;

  // Takes a direction and a price.
  // Returns an index of the quote with given direction and price.
  // If the quote with given price doesn't exist, maximum size_t value (i.e. `std::numeric_limits<size_t>::max()`) is returned.
  size_t index_by_price(Dir dir, Price price) const;

  // Takes a direction and a price.
  // Returns a volume of the quote with given direction and price.
  Amount volume_by_price(Dir dir, Price price) const {
    return quote_by_price(dir, price).volume();
  }

  // Takes a direction.
  // Returns the best price in order book with given direction.
  // Note: if the quote doesn't exist, corresponding default_quote_price is returned.
  Price best_price(Dir dir) const {
    return price_by_index(dir, 0);
  }

  // Takes a direction.
  // Returns a volume of the quote with the best price by given direction.
  Amount best_volume(Dir dir) const {
    return volume_by_index(dir, 0);
  }

  // Takes a direction.
  // Returns all quotes with given direction as a QuotesHolder object.
  // QuotesHolder is a container, which allows you to iterate you through all quotes.
  // Please read more about QuotesHolder here: <https://docs.hftbattle.com/en/api/QuotesHolder.html>.
  QuotesHolder all_quotes(Dir dir) const;

  // Takes a direction.
  // Returns a number of non-empty quotes with given direction.
  size_t quotes_count(Dir dir) const;

  // Returns a maximum number of visible quote levels in the order book (it's the same for both directions).
  size_t depth() const {
    return snapshot_depth_;
  }

  // Returns the latest server time, when the order book has been updated, in microseconds.
  Microseconds server_time() const {
    return last_moment_;
  }

  // Returns a reference to a SecurityOrdersSnapshot object containing all your current quotes.
  // Please read more about SecurityOrdersSnapshot here: <https://docs.hftbattle.com/en/api/SecurityOrdersSnapshot.html>.
  const SecurityOrdersSnapshot& orders() const {
    return orders_;
  }

  // Returns a half-sum of the best prices in both directions.
  Price middle_price() const {
    return middle_price_;
  }

  // Returns a minimum price step in the order book (the least possible difference between prices).
  Price min_step() const {
    return min_step_;
  }

  // Returns a distance between the best buy and best sell prices in minimum steps.
  size_t spread_in_min_steps() const {
    return spread_;
  }

  // Returns a fee per one executed lot.
  Decimal fee_per_lot() const;

  // Returns a number of the order book updates since the beginning of the trading session.
  // Note: research of the change of this value can be used to understand the market activity.
  size_t book_updates_count() const {
    return cnt_book_updates_;
  }

  virtual ~OrderBook() = default;

protected:
  friend class ParticipantStrategy;

  explicit OrderBook(size_t snapshot_depth);

  std::array<QuotesType, 2> quotes_;
  Microseconds last_moment_;
  Ticks last_tsc_;
  size_t snapshot_depth_;
  size_t cnt_book_updates_;
  size_t spread_;
  Price middle_price_;
  Price min_step_;
  SecurityId security_id_;
  SecurityOrdersSnapshot orders_;
};

}  // namespace hftbattle