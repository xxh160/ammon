#include <iostream>

int main(int argc, char **argv) {
  int t2 = 1 + 2 + 3;
  if (t2 >= 1) {
    t2 = 102 * 10086 / 2;
  }
  std::cout << t2 << std::endl;
  return 0;
}
