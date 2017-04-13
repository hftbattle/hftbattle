#include "participant_strategy.h"

using namespace hftbattle;

namespace {

class UserStrategy : public ParticipantStrategy {
public:
  UserStrategy(const JsonValue& config) :
      volume_(config["volume"].as<Amount>(2)),
      max_pos_(config["max_pos"].as<Amount>(1)),
      offset_(config["offset"].as<Price>(17)),
      volume_before_our_order_(config["volume_before_our_order"].as<Amount>(2)) {
    set_max_total_amount(max_pos_);
  }

  Amount max_available_order_amount(Amount pos, Dir dir) {
    Amount max_amount = std::min(max_pos_ - dir_sign(dir) * pos, volume_);
    return std::max(0, max_amount);
  }

  void trading_book_update(const OrderBook& order_book) override {
    const auto& orders = order_book.orders();
    Price middle_price = order_book.middle_price();
    Amount pos = executed_amount();

    add_chart_point("middle_price", middle_price);

    for (Dir dir : {BID, ASK}) {
      Amount accumulated_volume = 0;
      size_t idx = 0;
      for (; idx < order_book.depth(); ++idx) {
        accumulated_volume += order_book.volume_by_index(dir, idx);
        if (accumulated_volume >= volume_before_our_order_) {
          break;
        }
      }

      Price target_price = order_book.price_by_index(dir, idx) + dir_sign(dir) * order_book.min_step();
      Price diff = abs(target_price - order_book.best_price(opposite_dir(dir)));
      Amount order_amount = max_available_order_amount(pos, dir);

      if (orders.active_orders_count(dir) == 0) {
        if (order_amount > 0 && diff > offset_) {
          add_limit_order(dir, target_price, order_amount);
        }
      } else {
        Order* current_order = orders.orders_by_dir(dir).front();
        if (current_order->price() != target_price) {
          delete_order(current_order);
          if (order_amount > 0 && diff > offset_) {
            add_limit_order(dir, target_price, order_amount);
          }
        }
      }
    }
  }

private:
  Amount volume_;
  Amount max_pos_;
  Price offset_;
  Amount volume_before_our_order_;
};

}  // namespace

REGISTER_CONTEST_STRATEGY(UserStrategy, improved_strategy)
