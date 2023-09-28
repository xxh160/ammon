#include <random>
#include <string>
#include <util.h>

using namespace std;

// 生成 [x, y) 范围内的随机数
int randomNumber(int x, int y) {
  // 用来为随机数引擎提供种子
  static random_device rd;
  // 创建 Mersenne Twister 伪随机数生成器
  static mt19937 gen(rd());
  // 创建均匀分布（uniform distribution）
  uniform_int_distribution<> distrib(x, y - 1);
  // 使用生成器和分布输出随机数
  return distrib(gen);
}

// 生成长度为 n 的随机字符串
string randomStr(int n) {
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";
  string result;

  int x = 0, y = sizeof(alphanum) - 2;
  for (int i = 0; i < n; ++i) {
    result += alphanum[randomNumber(x, y)];
  }

  return result;
}
