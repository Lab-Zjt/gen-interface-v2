#include <tuple>
#include <string>
#include <unordered_map>
#include <map>
// Comment
/* Comment
 * Comment
 */

import "../example/interface.i";

namespace test;

interface Test<T1, T2, T3> extends ReadWriter {
  std::unordered_map<std::string, std::map<std::string, std::tuple<int, double, float>>>* m1(const std::tuple<std::string&, int*, const double* const> & t, const std::map<std::unordered_map<int, double>, std::string*>* m);
}

interface Test {
  int func2();
}