#include <iostream>
#include <thread>

#include "atomic_forward_list.h"

atomic_forward_list<int> list;

void work(int start) {
  for (int i = start; i < 10; i += 2) {
    list.push_front(i);
  }

  list.remove(start + 2);
}

int main() {
  std::thread thr1 = std::thread(work, 0);
  std::thread thr2 = std::thread(work, 1);

  thr1.join();
  thr2.join();

  for (auto i : list) {
    std::cout << i << std::endl;
  }
}
