/**
  * @file   example.cpp 
  * @author sora
  * @date   2020/6/6
  */

#include "interface.hpp"
#include "iterable.hpp"
#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>
class File {
  int fd;
 public:
  File() : fd(-1) {}
  File(int f) : fd(f) {}
  File(FILE *f) : fd(fileno(f)) {}
  std::tuple<ssize_t, int> read(void *buf, size_t size) { return {::read(0, buf, size), errno}; }
  std::tuple<ssize_t, int> write(const void *buf, size_t size) { return {::write(fd, buf, size), errno}; }
};

void writeString(example::Writer w, std::string_view str) {
  w.write(str.data(), str.size());
}

std::string readString(example::Reader r) {
  std::string s(1024, '\0');
  auto[c, err] = r.read(s.data(), s.size());
  if (err != 0) { perror("read"); }
  s.resize(c);
  return s;
}

void readWriteString(ReadWriter rw) {
  writeString(rw, readString(rw));
}



int main() {/*
  File f(1);
  readWriteString(f);*/
  using namespace std;
  string str("hello, world");
  string_view v("1234");
  vector<int> vec{1, 2, 3, 4, 5};
  vector<HasSize> hasSizeList{std::move(str), v, &vec};
  map<int, string> m{};
  hasSizeList.emplace_back(m);
  for (auto &h : hasSizeList) {
    std::cout << h.size() << std::endl;
  }
  auto sp = make_shared<string>("hello, world");
  auto up = make_unique<string_view>("1234");
  vector<HasSize> h{sp, up, make_shared<string>("12345"), make_unique<vector<int>>()};
  h.emplace_back(h);
  for (auto &hh : h) {
    cout << hh.size() << endl;
  }
}