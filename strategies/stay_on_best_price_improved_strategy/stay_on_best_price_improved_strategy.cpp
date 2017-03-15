#include "participant_strategy.h"

using namespace hftbattle;

class UserStrategy : public ParticipantStrategy {
public:
  explicit UserStrategy(const JsonValue& config) :
      min_volume_to_stay_on_best_(config["min_volume_to_stay_on_best"].as<int>(10)) { }

  void trading_book_update(const OrderBook& book) override {
    const auto& our_orders = book.orders();
    for (Dir dir : {BID, ASK}) {
      const Price best_price = book.best_price(dir);
      const Amount best_volume = book.best_volume(dir);
      const bool can_stay_on_best = best_volume >= min_volume_to_stay_on_best_;
      if (our_orders.active_orders_count(dir) == 0) {
        add_limit_order_if(dir, best_price, 1, can_stay_on_best);
      } else {
        auto first_order = our_orders.orders_by_dir(dir)[0];
        const bool on_best_price = first_order->price() == best_price;
        if (!on_best_price || !can_stay_on_best) {
          delete_order(first_order);
          add_limit_order_if(dir, best_price, 1, can_stay_on_best);
        }
      }
    }
  }

  void add_limit_order_if(Dir dir, Price price, Amount amount, bool condition) {
    if (condition) {
      add_limit_order(dir, price, amount);
    }
  }

private:
  Amount min_volume_to_stay_on_best_;
};

REGISTER_CONTEST_STRATEGY(UserStrategy, stay_on_best_price_improved_strategy)
