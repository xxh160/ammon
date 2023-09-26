#include <util.h>
#include <random>

using namespace std;

int randomNumber(int x, int y) {
  // 用来为随机数引擎提供种子
  random_device rd;
  // 创建 Mersenne Twister 伪随机数生成器
  mt19937 gen(rd());
  // 创建均匀分布（uniform distribution）
  uniform_int_distribution<> distrib(x, y - 1);
  // 使用生成器和分布输出随机数
  return distrib(gen);
}
