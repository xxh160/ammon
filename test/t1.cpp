#include <iostream>
#include <map>

using namespace std;

class T2 {};

class T1 {
public:
  T2 t2;
  int a;
  T1() {}
  T1(int a) : a(a) {}
};

int main(int argc, char **argv) {
  map<string, T1> m;
  m["hell"] = T1(1);
  int t1 = 1 - 2 - 3;
  cout << t1 << endl;
  return 0;
}
