auto find(const T& t) const {
  auto p = std::atomic_load(&m_head);
  while (p && p->t != t) {
    p = std::atomic_load(&p->next);
  }
  return reference(std::move(p));
}
