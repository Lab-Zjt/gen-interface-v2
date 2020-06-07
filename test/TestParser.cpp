/**
  * @file   TestParser.cpp 
  * @author sora
  * @date   2020/6/5
  */
#define private public
#include <iostream>
#include <fstream>
#include "../src/Parser.h"

bool testParseMethod() {
  std::vector<std::string> testList{
      "std::pair<std::vector<std::string>, std::unordered_map<std::string, std::string>> read(std::map<std::string, std::tuple<int, double, float>> arg1, std::stack<std::queue<std::deque<char>>> arg2);",
      "std::function<void(const std::unorderd_map<int, std::string>&)> operator()(std::string * stringRef, std::string* double);"
  };
  for (auto &test : testList) {
    if (auto result = Parser(test).parseMethod(); result) {
      std::cout << result->getRet() << " " << result->getName() << "(";
      for (auto &param : result->getParamList()) {
        std::cout << param.getType() << " " << param.getName() << ",";
      }
      std::cout << ")" << result->getSpecifier() << ";\n";
    } else {
      std::cerr << "Test " << __func__ << " failed\n";
      return false;
    }
  }
  std::cout << "Test " << __func__ << " success\n";
  return true;
}

bool testParseInterface() {
  std::vector<std::string> testList{
      "interface Test<T> extends T1<T>, T2<T> {\n"
      "std::pair<std::vector<std::string>, std::unordered_map<std::string, std::string>> read(std::map<std::string, std::tuple<int, double, float>> arg1, std::stack<std::queue<std::deque<char>>> arg2);\n"
      "std::function<void(const std::unorderd_map<int, std::string>&)> operator()(std::string * stringRef, std::string* double);\n"
      "}\n"
  };
  for (auto &test : testList) {
    if (auto result = Parser(test).parseInterface(); result) {
      std::cout << "interface" << result->getName() << " ";
      if (!result->getExtendsList().empty()) {
        std::cout << "extends ";
        for (auto &ext : result->getExtendsList()) {
          std::cout << ext << ", ";
        }
      }
      std::cout << "{\n";
      for (auto &method : result->getMethodList()) {
        std::cout << method.getRet() << " " << method.getName() << "(";
        for (auto &param : method.getParamList()) {
          std::cout << param.getType() << " " << param.getName() << ",";
        }
        std::cout << ")" << method.getSpecifier() << ";\n";
      }
      std::cout << "}\n";
    } else {
      std::cerr << "Test " << __func__ << " failed\n";
      return false;
    }
  }
  std::cout << "Test " << __func__ << " success\n";
  return true;
}

bool testFile() {
  std::vector<std::string> fileList{"../test/test.i"};
  for (auto &file : fileList) {
    std::ifstream ifs(file);
    std::string str{std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};
    Parser p(std::move(str));
    while (true) {
      auto result = p.parse();
      if (!result.success()) break;
      if (result.isComment()) {
        std::cout << "[COMMENT] " << result.getComment().comment_ << "\n";
      } else if (result.isImport()) {
        std::cout << "[IMPORT] " << result.getImport().import_ << "\n";
      } else if (result.isNamespace()) {
        std::cout << "[NAMESPACE] " << result.getNamespace().namespace_ << "\n";
      } else if (result.isInterface()) {
        std::cout << "[INTERFACE] " << result.getInterface().getName() << "\n";
      }
    }
  }
  return true;
}

int main(int argc, char *argv[]) {
  testFile();
}
