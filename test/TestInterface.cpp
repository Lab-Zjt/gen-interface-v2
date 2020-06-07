/**
  * @file   TestInterface.cpp 
  * @author sora
  * @date   2020/6/6
  */

#include "../src/Runtime.h"
#include <vector>
int main() {
  using namespace std;
  Proxy<string> s(make_shared<vector<int>>());


}