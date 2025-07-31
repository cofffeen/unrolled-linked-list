#pragma once

#include <memory>

template <typename T, size_t NodeMaxSize, typename Allocator> struct Node {
  using value_type = T;

  Node(const Allocator &alloc) : allocator(alloc) {}
  ~Node() = default;

  Allocator allocator;
  Node *next = nullptr;
  Node *prev = nullptr;
  size_t size = 0;
  alignas(alignof(value_type)) char data[NodeMaxSize * sizeof(value_type)];
};
