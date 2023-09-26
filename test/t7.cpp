int globalVar = 0;

class MyClass {
public:
  int classVar = 5;

  void myMethod() {
    int methodVar;
    int a = 1;
    methodVar = a + 2;
    classVar = methodVar + 3;
    if (a) {
      int c = 10;
      a = a + 44;
    }
    int someResult = classVar * 2;
    globalVar = a * 4;
    if (a) {
      int b = 10;
      a = a + 44;
    }
    int d = 8;
    ++d;
    a = d + 1 + methodVar;
  }
};
