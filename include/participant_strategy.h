#pragma once
#include "deal.h"
#include "execution_report.h"
#include "order.h"
#include "order_book.h"
#include "strategy_maker.h"
#include "base/common_enums.h"
#include "base/json.h"
#include "base/log.h"
#include "base/constants.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace hftbattle {

class Strategy;
class DataFeedSnapshot;

namespace simulator {

class RoyalManagerBase;
class RoyalManagerContest;

}  // namespace simulator

// A wrapper class for your strategy, which interacts with a trading simulator.
class ParticipantStrategy : public LoggableComponent {
public:
  // This method is called after getting a new order book of a trading instrument.
  virtual void trading_book_update(const OrderBook& /*order_book*/) { }

  // This method is called after getting new deals on a trading instrument.
  virtual void trading_deals_update(std::vector<Deal>&& /*deals*/) { }

  // This method is called after getting a report on your order execution.
  virtual void execution_report_update(const ExecutionReport& /*execution_report*/) { }

  // Your current trading order book.
  // Note: you cannot get trading order book before first `trading_book_update` call.
  const OrderBook& trading_book() const {
    return *trading_book_;
  }

  // Takes a direction, a price and an amount of the new order.
  // Places the new limit order.
  // Returns bool value — was the placement successful or not.
  // Note: the order can be rejected if certain restrictions are not met.
  // Please read about it here: <https://docs.hftbattle.com/en/HFAQ.html#simulator>
  bool add_limit_order(Dir dir, Price price, Amount amount) const;

  // Takes a direction, a price and an amount of the new order.
  // Places the new IOC (Immediate-Or-Cancel) order.
  // Returns bool value — was the placement successful or not.
  // Note: the order can be rejected if certain restrictions are not met.
  // Please read about it here: <https://docs.hftbattle.com/en/HFAQ.html#simulator>
  bool add_ioc_order(Dir dir, Price price, Amount amount) const;

  // Takes a pointer to your order.
  // Sends a request to remove this order from the auction.
  // Note: the deletion is not executed instantly.
  // You can read more about restrictions here: <https://docs.hftbattle.com/en/simulator/restrictions.html>.
  void delete_order(Order* order) const;

  // Takes a direction.
  // Sends a request to remove all your orders with given direction.
  void delete_all_orders_at_dir(Dir dir) const;

  // Takes a direction and a price.
  // Sends a request to remove all your orders with given direction and price.
  void delete_all_orders_at_price(Dir dir, Price price) const;

  // Takes a pointer to your order.
  // Returns total number of lots queued before your order in the quote with given price.
  Amount amount_before_order(const Order* order) const;

  // Takes a direction and a price.
  // Returns total number of lots in your active orders with given direction and price.
  Amount volume_by_price(Dir dir, Price price) const;

  // Takes a line_name string (name of the chart), double or Decimal value, y_axis_type — side which Y axis will be drawn on and chart_number — the number of your chart.
  // Adds point to the chart at the current moment with given value.
  // Each two adjacent points are connected.
  // The resulting polygonal chain is your chart.
  // chart_number allows you to create several charts and line_name allows you to draw several lines on the single image.
  void add_chart_point(const std::string& line_name, Decimal value, ChartYAxisType y_axis_type = ChartYAxisType::Left, uint8_t chart_number = 1) const;

  // Returns current result of your strategy (your profit).
  // Executed and placed orders are taken into account.
  // When calculating the result, we assume that all active orders are executed at the opposite best price.
  Decimal current_result() const;

  // Returns current server time in microseconds.
  Microseconds server_time() const;

  // Returns a server time of struct tm type with seconds precision.
  // Note: server time in this format is useful to determine the time of the day.
  tm server_time_tm() const;

  // Takes desired value of maximum position — non-negative number which must not be greater than 100.
  // Sets this position as maximum and doesn't allow your strategy to exceed it.
  void set_max_total_amount(const Amount max_total_amount);

  // Takes non-positive number — desired minimum result.
  // If your strategy reaches this result, simulator automatically liquidates your position and stops your strategy.
  // Note: your position liquidation is not executed instantly, so your result may significantly differ from *stop_loss*.
  // You can read more here: <https://docs.hftbattle.com/en/HFAQ.html#simulator>
  void set_stop_loss_result(const Decimal stop_loss_result);

  // Returns your current position.
  // Only executed orders are taken into account.
  Amount executed_amount() const {
    return executed_amount_;
  }

  // Takes a pointer to the order.
  // Returns bool value — whether this order is yours or not.
  bool is_our(const Order* order) const;

  // Takes a reference to the deal.
  // Returns bool value — whether your order was matched in the deal.
  bool is_our(const Deal& deal) const;

  void fix_moment_in_viewer(const std::string& name);

  ParticipantStrategy();

  virtual ~ParticipantStrategy();

protected:
  const std::shared_ptr<OrderBook>& trading_book_snapshot() const {
    return trading_book_;
  }

private:
  void trade(std::shared_ptr<DataFeedSnapshot>&& snapshot);

  friend class Strategy;
  friend class simulator::RoyalManagerBase;
  friend class simulator::RoyalManagerContest;

  std::shared_ptr<OrderBook> trading_book_;
  const SecurityId trading_security_;
  std::unique_ptr<Strategy> strategy_;
  struct tm current_update_server_time_tm_;
  Amount max_executed_amount_;
  Amount executed_amount_;
  std::vector<std::pair<std::string, std::string>> viewer_links_;
};

#define REGISTER_CONTEST_STRATEGY(ClassName, file) \
_Pragma("clang diagnostic push") \
_Pragma("clang diagnostic ignored \"-Wreturn-type-c-linkage\"") \
extern "C" StrategyHolder make_ ## file(const JsonValue& config) { \
  return create_strategy<ClassName>(config); \
} \
_Pragma("clang diagnostic pop") \

}  // namespace hftbattle