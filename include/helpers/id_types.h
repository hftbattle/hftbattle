#pragma once
#include <limits>

namespace hftbattle {
namespace simulator {

enum class IdTypes {
  StrategyId,
  OrderBookId
};

template<IdTypes id_type>
class BaseId {
public:
  using base_type = uint8_t;

  explicit BaseId(size_t index) : id_(static_cast<base_type>(index)) { }

  operator base_type() const {
    return id_;
  }

  bool operator==(const BaseId& rhs) const {
    return id_ == rhs.id_;
  }

  static BaseId undefined() {
    return BaseId(std::numeric_limits<base_type>::max());
  }

private:
  base_type id_;
};

using StrategyId = BaseId<IdTypes::StrategyId>;
using OrderBookId = BaseId<IdTypes::OrderBookId>;

}  // namespace simulator

using simulator::StrategyId;

}  // namespace hftbattle
