#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
  vector<int> twoSum(vector<int> &&nums, int target) {
    int n = nums.size();
    for (int i = 0; i < n; ++i) {
      for (int j = i + 1; j < n; j++) {
        if (nums[i] + nums[j] == target) {
          return {i, j};
        }
      }
    }
    return {};
  }
};

int main() {
  Solution s;
  int a = 1;
  int *ptr = &--a;
  cout << 1 - *ptr + 1 + 2 - --a + 1 -a++ << endl;

  s.twoSum({1, 2, 3}, 5);
  return 0;
}
