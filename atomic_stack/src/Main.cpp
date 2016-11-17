#include <iostream>
#include <thread>

#include "atomic_stack.h"

atomic_stack<int> stack;

void work(int start) {
  for (int i = start; i < 10; i += 2) {
    stack.push(i);
  }

  while (stack.size() > 2) {
    stack.pop();
  }
}

int main() {
  std::thread thr1 = std::thread(work, 0);
  std::thread thr2 = std::thread(work, 1);

  thr1.join();
  thr2.join();

  while (!stack.empty()) {
    std::cout << *stack.top() << std::endl;
    stack.pop();
  }
}
