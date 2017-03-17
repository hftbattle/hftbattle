#include "participant_strategy.h"
#include <vector>

using namespace hftbattle;

namespace {

class UserStrategy : public ParticipantStrategy {
public:
  explicit UserStrategy(const JsonValue& /*config*/) { }

  void trading_book_update(const OrderBook& /*order_book*/) override { }

  void trading_deals_update(std::vector<Deal>&& /*deals*/) override { }

  void execution_report_update(const ExecutionReport& /*execution_report*/) override { }

  void signal_book_update(const OrderBook& /*order_book*/) override { }

  void signal_deals_update(std::vector<Deal>&& /*deals*/) override { }
};

}  // namespace

REGISTER_CONTEST_STRATEGY(UserStrategy, template_strategy)
