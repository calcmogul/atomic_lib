#pragma once

#include <atomic>
#include <memory>

template <class T>
class atomic_stack {
  struct Node {
    Node(const T& val) : t{val} {}
    Node(T&& val) : t{val} {}

    T t;
    std::shared_ptr<Node> next;
    std::atomic<size_t> size{1};  // container size
  };
  std::shared_ptr<Node> m_head;  // should be atomic

  class reference {
   public:
    reference(std::shared_ptr<Node> p) : m_p{p} {}
    T& operator*() { return m_p->t; }
    T* operator->() { return &m_p->t; }

   private:
    std::shared_ptr<Node> m_p;
  };

 public:
  atomic_stack() = default;
  atomic_stack(const atomic_stack&) = delete;
  atomic_stack& operator=(const atomic_stack&) = delete;

  auto top() { return reference(std::atomic_load(&m_head)); }

  const auto top() const { return reference(std::atomic_load(&m_head)); }

  void push(const T& t) {
    // Make a new node
    auto p = std::make_shared<Node>(t);

    // Point p's next pointer at head
    p->next = std::atomic_load(&m_head);

    // Update head to new node
    do {
      if (p->next) {
        p->size = p->next->size + 1;
      }
    } while (!std::atomic_compare_exchange_weak(&m_head, &p->next, p));
  }

  void push(T&& t) {
    // Make a new node
    auto p = std::make_shared<Node>(std::forward<T>(t));

    // Point p's next pointer at head
    p->next = std::atomic_load(&m_head);

    // Update head to new node
    do {
      p->size = p->next->size + 1;
    } while (!std::atomic_compare_exchange_weak(&m_head, &p->next, p));
  }

  template <class... Args>
  void emplace(Args&&... args) { push(std::move(T(args...))); }

  void pop() {
    auto p = std::atomic_load(&m_head);

    /* Update head to next pointer; no need to decrement Node::size since
     * the next pointer already has the correct size as if it was head due
     * to how it was stored in push_front() (size decrements from head
     * through link chain to one in last element)
     */
    while (p && !std::atomic_compare_exchange_weak(&m_head, &p, p->next)) {}
  }

  bool empty() const { return std::atomic_load(&m_head) == nullptr; }

  size_t size() const {
    auto p = std::atomic_load(&m_head);
    if (p) {
      return p->size;
    } else {
      return 0;
    }
  }

  void swap(atomic_stack& obj) noexcept {
    auto p = std::atomic_load(&m_head);
    auto rhs = std::atomic_exchange(&obj.m_head, p);
    std::atomic_store(&m_head, p);
  }
};
