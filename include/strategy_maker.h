#pragma once
#include <memory>
#include <utility>

namespace hftbattle {

class Strategy;
class JsonValue;
class ParticipantStrategy;

using StrategyHolder = std::unique_ptr<ParticipantStrategy>;

template<typename SpecificStrategy, typename... Args>
StrategyHolder create_strategy(Args&& ... args) {
  return std::make_unique<SpecificStrategy>(std::forward<Args>(args)...);
}

using MakeStrategy = StrategyHolder (*)(const JsonValue& config);

}  // namespace hftbattle
