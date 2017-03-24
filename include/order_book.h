#pragma once
#include "quote.h"
#include "quotes_holder.h"
#include "security_orders_snapshot.h"
#include "base/common_enums.h"
#include "base/constants.h"
#include "base/perf_time.h"
#include <array>

namespace hftbattle {

// docs.order_book.class
class OrderBook {
public:
  // docs.order_book.quote_by_index
  const Quote& quote_by_index(Dir dir, size_t index) const;

  // docs.order_book.price_by_index
  Price price_by_index(Dir dir, size_t index) const {
    return quote_by_index(dir, index).price();
  }

  // docs.order_book.volume_by_index
  Amount volume_by_index(Dir dir, size_t index) const {
    return quote_by_index(dir, index).volume();
  }

  // docs.order_book.quote_by_price
  const Quote& quote_by_price(Dir dir, Price price) const;

  // docs.order_book.index_by_price
  size_t index_by_price(Dir dir, Price price) const;

  // docs.order_book.volume_by_price
  Amount volume_by_price(Dir dir, Price price) const {
    return quote_by_price(dir, price).volume();
  }

  // docs.order_book.best_price
  Price best_price(Dir dir) const {
    return price_by_index(dir, 0);
  }

  // docs.order_book.best_volume
  Amount best_volume(Dir dir) const {
    return volume_by_index(dir, 0);
  }

  // docs.order_book.all_quotes
  QuotesHolder all_quotes(Dir dir) const;

  // docs.order_book.quotes_count
  size_t quotes_count(Dir dir) const;

  // docs.order_book.depth
  size_t depth() const {
    return snapshot_depth_;
  }

  // docs.order_book.server_time
  Microseconds server_time() const {
    return last_moment_;
  }

  // docs.order_book.orders
  const SecurityOrdersSnapshot& orders() const {
    return orders_;
  }

  // docs.order_book.middle_price
  Price middle_price() const {
    return middle_price_;
  }

  // docs.order_book.min_step
  Price min_step() const {
    return min_step_;
  }

  // docs.order_book.spread_in_min_steps
  size_t spread_in_min_steps() const {
    return spread_;
  }

  // docs.order_book.fee_per_lot
  Decimal fee_per_lot() const;

  // docs.order_book.book_updates_count
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
