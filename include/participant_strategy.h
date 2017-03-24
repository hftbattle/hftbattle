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

// docs.participant_strategy.class
class ParticipantStrategy : public LoggableComponent {
public:
  // docs.participant_strategy.trading_book_update
  virtual void trading_book_update(const OrderBook& /*order_book*/) { }

  // docs.participant_strategy.trading_deals_update
  virtual void trading_deals_update(std::vector<Deal>&& /*deals*/) { }

  // docs.participant_strategy.execution_report_update
  virtual void execution_report_update(const ExecutionReport& /*execution_report*/) { }

  // docs.participant_strategy.trading_book
  const OrderBook& trading_book() const {
    return *trading_book_;
  }

  // docs.participant_strategy.add_limit_order
  bool add_limit_order(Dir dir, Price price, Amount amount) const;

  // docs.participant_strategy.add_ioc_order
  bool add_ioc_order(Dir dir, Price price, Amount amount) const;

  // docs.participant_strategy.delete_order
  void delete_order(Order* order) const;

  // docs.participant_strategy.delete_all_orders_at_dir
  void delete_all_orders_at_dir(Dir dir) const;

  // docs.participant_strategy.delete_all_orders_at_price
  void delete_all_orders_at_price(Dir dir, Price price) const;

  // docs.participant_strategy.amount_before_order
  Amount amount_before_order(const Order* order) const;

  // docs.participant_strategy.volume_by_price
  Amount volume_by_price(Dir dir, Price price) const;

  // docs.participant_strategy.add_chart_point
  void add_chart_point(const std::string& line_name, Decimal value, ChartYAxisType y_axis_type = ChartYAxisType::Left, uint8_t chart_number = 1) const;

  // docs.participant_strategy.current_result
  Decimal current_result() const;

  // docs.participant_strategy.server_time
  Microseconds server_time() const;

  // docs.participant_strategy.server_time_tm
  tm server_time_tm() const;

  // docs.participant_strategy.set_max_total_amount
  void set_max_total_amount(const Amount max_total_amount);

  // docs.participant_strategy.set_stop_loss_result
  void set_stop_loss_result(const Decimal stop_loss_result);

  // docs.participant_strategy.executed_amount
  Amount executed_amount() const {
    return executed_amount_;
  }

  // docs.participant_strategy.is_our.order
  bool is_our(const Order* order) const;

  // docs.participant_strategy.is_our.deal
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
