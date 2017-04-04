#pragma once
#include <functional>
#include <map>
#include <memory>

namespace hftbattle {

class Quote;

using QuotesType = std::map<Price, std::unique_ptr<Quote>,
    std::function<bool(const Price&, const Price&)>>;

// This class allows you to iterate through the quotes.
// Quotes are sorted by price in descending order for BID (Buy) and in ascending order for ASK (Sell).
class QuotesHolder {
public:
  using BaseConstIterator = QuotesType::const_iterator;
  using BaseConstReverseIterator = QuotesType::const_reverse_iterator;

  class QuotesHolderIterator : public BaseConstIterator {
  public:
    using value_type = Quote;

    explicit QuotesHolderIterator(BaseConstIterator it) : BaseConstIterator(it) { }

    const value_type* operator->() const {
      return &**this;
    }

    const value_type& operator*() const {
      return *this->BaseConstIterator::operator->()->second;
    }
  };

  class QuotesHolderReverseIterator: public BaseConstReverseIterator {
  public:
    using value_type = Quote;
    explicit QuotesHolderReverseIterator(BaseConstReverseIterator it): BaseConstReverseIterator(it) {}

    const value_type* operator->() const {
      return &**this;
    }

    const value_type& operator*() const {
      return *this->BaseConstReverseIterator::operator->()->second;
    }
  };

  using const_iterator = QuotesHolderIterator;
  using const_reverse_iterator = QuotesHolderReverseIterator;

  explicit QuotesHolder(const QuotesType* quotes) : quotes_(quotes) { }

  // An iterator pointing to the first element in the collection.
  const_iterator begin() const {
    return QuotesHolderIterator(quotes_->begin());
  }

  // An iterator referring to the past-the-end element in the collection.
  const_iterator end() const {
    return QuotesHolderIterator(quotes_->end());
  }

  // A reverse iterator pointing to the last element.
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(quotes_->rbegin());
  }

  // A reverse iterator pointing to the theoretical element preceding the first element.
  const_reverse_iterator rend() const {
    return const_reverse_iterator(quotes_->rend());
  }

private:
  const QuotesType* quotes_;
};

}  // namespace hftbattle