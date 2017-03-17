#include "participant_strategy.h"

using namespace hftbattle;

namespace {

class UserStrategy : public ParticipantStrategy {
public:
  UserStrategy(const JsonValue& config) :
      volume_(config["volume"].as<Amount>(1)),
      max_pos_(config["max_pos"].as<Amount>(1)),
      offset_(config["offset"].as<Price>(18)) {
    set_max_total_amount(max_pos_);
  }

  Amount amount_available(const Amount pos, const Dir dir) {
    auto max_volume = std::min(max_pos_ - dir_sign(dir) * pos, volume_);
    return std::max(0, max_volume);
  }

  void trading_book_update(const OrderBook& order_book) override {
    const auto& orders = order_book.orders();
    auto middle_price = order_book.middle_price();
    auto pos = executed_amount();

    add_chart_point("middle_price", middle_price);

    for (Dir dir : {BID, ASK}) {
      auto target_price = middle_price - dir_sign(dir) * offset_;
      auto order_amount = amount_available(pos, dir);

      if (orders.active_orders_count(dir) == 0) {
        if (order_amount > 0) {
          add_limit_order(dir, target_price, order_amount);
        }
      } else {
        auto current_order = orders.orders_by_dir(dir).front();
        if (current_order->price() != target_price) {
          delete_order(current_order);
          if (order_amount > 0) {
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
};

}  // namespace

REGISTER_CONTEST_STRATEGY(UserStrategy, user_strategy)
