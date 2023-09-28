#include <iostream>

using namespace std;

class A {
public:
  virtual void foo() { cout << "A" << endl; }
};

class B : public A {
public:
  void foo() override { cout << "B" << endl; }
};

class C : public B {
public:
  void foo() override { cout << "C" << endl; }
};

int main() {
  A *a = new C();
  B *b = new C();
  a->foo();
  b->foo();

  A* as[] = {new A(), new B(), new C()};

  for (int i = 0; i < 3; ++i) {
    as[i]->foo();
  }
}
