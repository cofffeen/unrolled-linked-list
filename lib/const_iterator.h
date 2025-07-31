#pragma once

#include "node.h"
#include "unrolled_list.h"

template <typename T, size_t NodeMaxSize, typename Allocator>
class unrolled_list;

template <typename Iterator> 
class ConstIterator {
public:
  using iterator_category = Iterator::iterator_category;
  using value_type = const Iterator::value_type;
  using allocator_type = Iterator::allocator_type;
  using difference_type = Iterator::difference_type;
  using pointer = const Iterator::value_type *;
  using reference = const Iterator::value_type &;
  using NodePtr = Iterator::NodePtr;
  using size_type = Iterator::size_type;

  friend class unrolled_list<value_type, Iterator::kNodeMaxSize,
                             allocator_type>;

  ConstIterator() = default;

  ConstIterator(const ConstIterator &other) { iterator_ = other.iterator_; }

  ConstIterator(Iterator iterator) : iterator_(iterator) {}

  ~ConstIterator() = default;

  ConstIterator &operator=(const ConstIterator &other) {
    iterator_ = other.iterator_;
    return *this;
  }

  reference operator*() const { return *iterator_; }

  pointer operator->() const { return iterator_.operator->(); }

  bool operator==(const ConstIterator &other) const {
    return (iterator_ == other.iterator_);
  }

  bool operator!=(const ConstIterator &other) const { return !(*this == other); }

  ConstIterator &operator++() {
    ++iterator_;
    return *this;
  }

  ConstIterator operator++(int) {
    ConstIterator<Iterator> copy = *this;
    ++(*this);
    return copy;
  }

  ConstIterator &operator--() {
    --iterator_;

    return *this;
  }

  ConstIterator operator--(int) {
    ConstIterator<Iterator> copy = *this;
    --(*this);
    return copy;
  }

private:
  Iterator iterator_;
};
