/**
  * @file   Interface.h 
  * @author sora
  * @date   2020/6/5
  */

#ifndef GEN_INTERFACE_V2_SRC_INTERFACE_H_
#define GEN_INTERFACE_V2_SRC_INTERFACE_H_
#include "Method.h"
// Interface object
class Interface {
  std::string name_;
  std::vector<Method> methodList_;
  std::vector<std::string> extendsList_;
 public:
  const std::string &getName() const { return name_; }
  void setName(const std::string &name) { name_ = name; }
  const std::vector<Method> &getMethodList() const { return methodList_; }
  void addMethod(const Method &method) { methodList_.push_back(method); }
  const std::vector<std::string> getExtendsList() const { return extendsList_; }
  void addExtends(const std::string &extends) { extendsList_.push_back(extends); }
};

#endif //GEN_INTERFACE_V2_SRC_INTERFACE_H_
