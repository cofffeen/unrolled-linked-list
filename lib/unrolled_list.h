#pragma once

#include "const_iterator.h"
#include "iterator.h"
#include <limits>
#include <memory>

static_assert(std::bidirectional_iterator<ConstIterator<Iterator<int, 10, std::allocator<int>>>>);

template <typename T, size_t NodeMaxSize = 10,
          typename Allocator = std::allocator<T>>
class unrolled_list {
public:
  using value_type = T;
  using pointer = T *;
  using reference = T &;
  using const_reference = const T &;
  using difference_type = ptrdiff_t;
  using size_type = size_t;
  using allocator_type = Allocator;
  using NodePtr = Node<value_type, NodeMaxSize, allocator_type> *;
  using node_alloc =
      typename std::allocator_traits<Allocator>::template rebind_alloc<
          Node<value_type, NodeMaxSize, allocator_type>>;
  using iterator = Iterator<value_type, NodeMaxSize, allocator_type>;
  using const_iterator = ConstIterator<iterator>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  unrolled_list() = default;

  unrolled_list(const Allocator &allocator) : allocator_(allocator) {}

  unrolled_list(const unrolled_list &other)
      : allocator_(other.allocator), node_allocator_(other.node_allocator_) {
    try {
      for (auto it = other.begin(); it != other.end(); ++it) {
        push_back(*it);
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  unrolled_list(size_type n, const value_type &value) {
    try {
      for (size_type i = 0; i < n; ++i) {
        push_back(value);
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  template <typename InputIterator>
  unrolled_list(InputIterator begin, InputIterator end) {
    try {
      while (begin != end) {
        push_back(*begin);
        ++begin;
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  template <typename InputIterator>
  unrolled_list(InputIterator begin, InputIterator end,
                const Allocator &allocator) {
    try {
      allocator_ = allocator;
      while (begin != end) {
        push_back(*begin);
        ++begin;
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  unrolled_list(const unrolled_list &other, const Allocator &allocator)
      : allocator_(allocator) {
    try {
      for (const_iterator value : other) {
        push_back(*value);
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  unrolled_list(const std::initializer_list<value_type> &other) {
    try {
      unrolled_list(other.begin(), other.end());
    } catch (...) {
      clear();
      throw;
    }
  }

  ~unrolled_list() { clear(); }

  unrolled_list &operator=(const unrolled_list &other) {
    allocator_ = other.allocator_;
    node_allocator_ = other.allocator_;
    try {
      for (const_iterator it : other) {
        push_back(*it);
      }
      return *this;
    } catch (...) {
      clear();
      throw;
    }
  }

  allocator_type get_allocator() { return allocator_; }

  iterator begin() { return iterator(front_); }

  iterator end() { return iterator(nullptr, 0); }

  const_iterator begin() const { return const_iterator(iterator(front_)); }

  const_iterator end() const { return const_iterator(); }

  const_iterator cbegin() const { return begin(); }

  const_iterator cend() const { return end(); }

  reverse_iterator rbegin() { return begin(); }

  reverse_iterator rend() { return end(); }

  const_reverse_iterator rbegin() const { return cbegin(); }

  const_reverse_iterator rend() const { return cend(); }

  const_reverse_iterator crbegin() const { return rbegin(); }

  const_reverse_iterator crend() const { return rend(); }

  bool operator==(const unrolled_list &other) {
    for (iterator it = begin(), it_other = other.begin();
         it != end() && it_other != other.end(); ++it, ++it_other) {
      if (it != it_other) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const unrolled_list &other) { return !(*this == other); }

  reference front() { return *begin(); }

  const_reference front() const { return *cbegin(); }

  reference back() { return *iterator(back_, back_->size - 1); }

  const_reference back() const {
    return *const_iterator(iterator(back_, back_->size - 1));
  }

  void swap(unrolled_list &other) {
    std::swap(front_, other.front_);
    std::swap(back_, other.back_);
    std::swap(size_, other.size_);
    std::swap(allocator_, other.allocator_);
    std::swap(node_allocator_, other.node_allocator_);
  }

  bool empty() { return (size_ == 0); }

  size_type size() { return size_; }

  size_type max_size() { return std::numeric_limits<size_type>::max(); }

  iterator insert(const_iterator pos, const value_type &value) {
    return Insert<const_iterator>(pos, value);
  }

  iterator insert(iterator pos, const value_type &value) {
    return Insert<iterator>(pos, value);
  }

  iterator insert(iterator pos, size_type n, const value_type &value) {
    iterator old_pos = pos;
    try {
      for (size_type i = 0; i < n; ++i) {
        Insert<iterator>(pos, value);
        ++pos;
      }
      return --pos;
    } catch (...) {
      for (iterator it = old_pos; it != pos; ++it) {
        erase(it);
      }
      throw;
    }
  }

  iterator insert(const_iterator pos, size_type n, const value_type &value) {
    const_iterator old_pos = pos;
    try {
      for (size_type i = 0; i < n; ++i) {
        ++pos;
        Insert<const_iterator>(pos, value);
      }
      return --pos;
    } catch (...) {
      for (const_iterator it = old_pos; it != pos; ++it) {
        erase(it);
      }
      throw;
    }
  }

  iterator erase(iterator pos) noexcept { return Erase<iterator>(pos); }

  iterator erase(const_iterator pos) noexcept {
    return Erase<const_iterator>(pos);
  }

  iterator erase(iterator begin, iterator end) noexcept {
    while (begin != end) {
      Erase<iterator>(begin);
      ++begin;
    }
    return --begin;
  }

  iterator erase(const_iterator begin, const_iterator end) noexcept {
    while (begin != end) {
      Erase<const_iterator>(begin);
      ++begin;
    }
    return --begin;
  }

  void clear() noexcept {
    while (size_ > 0) {
      pop_back();
    }
  }

  void push_back(const value_type &value) {
    NodePtr old_node = back_;
    NodePtr node = nullptr;
    size_type old_size = size_;

    try {
      if (size_ == 0) {
        node = AllocateNode(value);
        front_ = node;
        ++front_->size;
        ++size_;
        back_ = front_;
      } else if (back_->size == NodeMaxSize) {
        node = AllocateNode(value);

        ++node->size;
        ++size_;

        node->prev = back_;
        back_->next = node;
        back_ = node;
      } else {
        ConstructNode(back_, value, back_->size);
        ++back_->size;
        ++size_;
      }
    } catch (...) {
      if (node != nullptr) {
        if (back_ == node) {
          back_ = old_node;
        }
        DeallocateNode(node);
      }
      size_ = old_size;
      throw;
    }
  }

  void push_front(const value_type &value) {
    NodePtr old_node = front_;
    NodePtr new_node = nullptr;
    size_type old_size = size_;

    try {
      if (size_ == 0) {
        front_ = AllocateNode(value);
        ++front_->size;
        ++size_;
        back_ = front_;
      } else if (front_->size == NodeMaxSize) {
        new_node = AllocateNode(value);

        ++new_node->size;
        ++size_;

        new_node->next = front_;
        front_->prev = new_node;
        front_ = new_node;
      } else {
        ShiftDataRight(front_, 0);
        ConstructNode(front_, value, 0);

        ++front_->size;
        ++size_;
      }
    } catch (...) {
      if (new_node != nullptr) {
        if (front_ == new_node) {
          front_ = old_node;
        }
        DeallocateNode(new_node);
      }
      size_ = old_size;
      throw;
    }
  }

  void pop_back() noexcept {
    if (back_ == nullptr) {
      return;
    }

    NodePtr node = back_;

    value_type *last_element =
        reinterpret_cast<value_type *>(node->data) + node->size;
    std::allocator_traits<allocator_type>::destroy(node->allocator,
                                                   last_element);
    --node->size;
    --size_;

    if (node->size == 0) {
      if (node->prev != nullptr) {
        node->prev->next = nullptr;
        back_ = node->prev;
      } else {
        front_ = nullptr;
        back_ = nullptr;
      }

      DeallocateNode(node);
    }
  }

  void pop_front() noexcept {
    if (front_ == nullptr) {
      return;
    }

    NodePtr node = front_;

    value_type *first_element = reinterpret_cast<value_type *>(node->data);
    std::allocator_traits<allocator_type>::destroy(node->allocator,
                                                   first_element);
    --node->size;
    --size_;

    ShiftDataLeft(node, 0);

    if (node->size == 0) {
      if (node->next != nullptr) {
        node->next->prev = nullptr;
        front_ = node->next;
      } else {
        front_ = nullptr;
        back_ = nullptr;
      }

      DeallocateNode(node);
    }
  }

private:
  NodePtr front_ = nullptr;
  NodePtr back_ = nullptr;
  size_type size_ = 0;
  Allocator allocator_;
  node_alloc node_allocator_{allocator_};

  void ShiftDataRight(NodePtr &node, const size_type &ind) {
    value_type *data = reinterpret_cast<value_type *>(node->data);
    size_type i = 0;
    try {
      for (i = node->size; i > ind; --i) {
        std::allocator_traits<allocator_type>::construct(node->allocator,
                                                         data + i, data[i - 1]);
        std::allocator_traits<allocator_type>::destroy(node->allocator,
                                                       data + i - 1);
      }
    } catch (...) {
      ShiftDataLeft(node, i);
      throw;
    }
  }

  void ShiftDataLeft(NodePtr &node, const size_type &ind) noexcept {
    value_type *data = reinterpret_cast<value_type *>(node->data);
    for (size_type i = ind; i < node->size; ++i) {
      std::allocator_traits<allocator_type>::construct(node->allocator,
                                                       data + i, data[i + 1]);
      std::allocator_traits<allocator_type>::destroy(node->allocator,
                                                     data + i + 1);
    }
  }

  void ConstructNode(NodePtr &node, const value_type &value, size_type ind) {
    NodePtr old_node = node;
    value_type *element = reinterpret_cast<value_type *>(node->data) + ind;
    try {
      std::allocator_traits<allocator_type>::construct(node->allocator, element,
                                                       value);
    } catch (...) {
      if (node != old_node) {
        std::allocator_traits<allocator_type>::destroy(node->allocator,
                                                       element);
        node = old_node;
      }
      throw;
    }
  }

  NodePtr AllocateNode(const value_type &value) {
    NodePtr node = nullptr;
    try {
      node = std::allocator_traits<node_alloc>::allocate(node_allocator_, 1);
      std::allocator_traits<node_alloc>::construct(node_allocator_, node,
                                                   allocator_);

      ConstructNode(node, value, node->size);

      return node;
    } catch (...) {
      if (node != nullptr) {
        DeallocateNode(node);
      }
      throw;
    }
  }

  void DeallocateNode(NodePtr &node) {
    if (node == nullptr) {
      return;
    }

    value_type *element = reinterpret_cast<value_type *>(node->data);

    for (size_type i = 0; i < node->size; ++i) {
      std::allocator_traits<allocator_type>::destroy(node->allocator,
                                                     element + i);
    }

    size_ -= node->size;

    std::allocator_traits<node_alloc>::destroy(node_allocator_, node);
    std::allocator_traits<node_alloc>::deallocate(node_allocator_, node, 1);
  }

  template <typename IteratorType>
  iterator Insert(IteratorType pos, const value_type &value) {
    NodePtr node = pos.current_node_;
    size_type ind = pos.current_ind_;

    NodePtr new_node = nullptr;

    try {
      if (node == nullptr) {
        push_back(value);
        return iterator(back_, back_->size - 1);
      }

      if (node->size < NodeMaxSize) {
        ShiftDataRight(node, ind);
        ConstructNode(node, value, ind);
        ++node->size;
        ++size_;

        return iterator(node, ind);
      }

      new_node =
          std::allocator_traits<node_alloc>::allocate(node_allocator_, 1);
      std::allocator_traits<node_alloc>::construct(node_allocator_, new_node,
                                                   node->allocator);

      size_type middle = node->size / 2;
      value_type *data = reinterpret_cast<value_type *>(node->data);
      value_type *new_data = reinterpret_cast<value_type *>(new_node->data);

      for (size_type i = middle; i < node->size; ++i) {
        std::allocator_traits<allocator_type>::construct(
            new_node->allocator, new_data + i - middle, data[i]);
        std::allocator_traits<allocator_type>::destroy(node->allocator,
                                                       data + i);
      }

      new_node->size = NodeMaxSize - middle;
      node->size = middle;

      new_node->next = node->next;
      if (node->next != nullptr) {
        node->next->prev = new_node;
      }
      node->next = new_node;
      new_node->prev = node;

      if (node == back_) {
        back_ = new_node;
      }

      if (ind < node->size) {
        ShiftDataRight(node, ind);
        ConstructNode(node, value, ind);
        ++node->size;
        ++size_;

        return iterator(node, ind);
      }
      ind -= node->size;
      ShiftDataRight(new_node, ind);
      ConstructNode(new_node, value, ind);
      ++new_node->size;
      ++size_;

      return iterator(new_node, ind);
    } catch (...) {
      if (node != nullptr) {
        if (node->prev != nullptr) {
          node->prev->next = node->next;
        }
        if (node->next != nullptr) {
          node->next->prev = node->prev;
        }
        if (node == back_) {
          back_ = node->prev;
        }

        DeallocateNode(node);
      }
      throw;
    }
  }

  template <typename IteratorType> 
  iterator Erase(IteratorType pos) noexcept {
    NodePtr node = pos.current_node_;
    size_type ind = pos.current_ind_;

    value_type *element = reinterpret_cast<value_type *>(node->data) + ind;
    std::allocator_traits<allocator_type>::destroy(node->allocator, element);
    --node->size;
    --size_;
    if (node->size == 0) {
      if (node->prev != nullptr) {
        node->prev->next = node->next;
      }
      if (node->next != nullptr) {
        node->next->prev = node->prev;
      }

      DeallocateNode(node);
    } else {
      ShiftDataLeft(node, ind);
    }

    return pos;
  }
};
