#pragma once

#include <iostream>
#include <concepts>
#include <iterator>

#include "node.h"
#include "unrolled_list.h"

template <typename T, size_t NodeMaxSize, typename Allocator>
class unrolled_list;

template <typename T, size_t NodeMaxSize, typename Allocator> 
class Iterator {
public:
  friend class unrolled_list<T, NodeMaxSize, Allocator>;

  static constexpr size_t kNodeMaxSize = NodeMaxSize;

  using NodePtr = Node<T, NodeMaxSize, Allocator> *;
  using value_type = T;
  using allocator_type = Allocator;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = ptrdiff_t;
  using pointer = value_type *;
  using reference = value_type &;
  using size_type = size_t;

  Iterator() {
    current_node_ = nullptr;
    current_ind_ = 0;
  }
  Iterator(const Iterator &other) {
    current_node_ = other.current_node_;
    current_ind_ = other.current_ind_;
  }

  ~Iterator() = default;

  Iterator &operator=(const Iterator &other) {
    current_node_ = other.current_node_;
    current_ind_ = other.current_ind_;
    return *this;
  }

  reference operator*() {
    return *(reinterpret_cast<value_type *>(current_node_->data) +
             current_ind_);
  }

  reference operator*() const {
    return *(reinterpret_cast<value_type *>(current_node_->data) +
             current_ind_);
  }

  pointer operator->() {
    return reinterpret_cast<value_type *>(current_node_->data) + current_ind_;
  }

  bool operator==(const Iterator &other) const {
    return (current_node_ == other.current_node_) &&
           (current_ind_ == other.current_ind_);
  }

  bool operator!=(const Iterator &other) const { return !(*this == other); }

  Iterator &operator++() {
    if (current_node_ != nullptr) {
      if (current_ind_ + 1 < current_node_->size) {
        ++current_ind_;
      } else {
        current_node_ = current_node_->next;
        current_ind_ = 0;
      }
    }

    return *this;
  }

  Iterator operator++(int) {
    Iterator<value_type, NodeMaxSize, Allocator> copy = *this;
    ++(*this);
    return copy;
  }

  Iterator &operator--() {
    if (current_node_ != nullptr) {
      if (current_ind_ > 0) {
        --current_ind_;
      } else {
        current_node_ = current_node_->prev;
        current_ind_ = current_node_->size - 1;
      }
    } else {
      current_ind_ = 0;
    }

    return *this;
  }

  Iterator operator--(int) {
    Iterator<value_type, NodeMaxSize, Allocator> copy = *this;
    --(*this);
    return copy;
  }

private:
  NodePtr current_node_;
  size_type current_ind_;

  Iterator(NodePtr node, size_type ind) {
    current_node_ = node;
    current_ind_ = ind;
  }
  Iterator(NodePtr node) {
    current_node_ = node;
    current_ind_ = 0;
  }
};

static_assert(std::bidirectional_iterator<Iterator<int, 10, std::allocator<int>>>);
