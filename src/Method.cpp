/**
  * @file   Method.cpp 
  * @author sora
  * @date   2020/6/5
  */

#include "Method.h"
const std::string &Method::getName() const {
  return name_;
}
void Method::setName(const std::string &name) {
  name_ = name;
}
const std::string &Method::getRet() const {
  return ret_;
}
void Method::setRet(const std::string &ret) {
  ret_ = ret;
}
const std::vector<Param> &Method::getParamList() const {
  return paramList_;
}
void Method::addParam(const Param &param) {
  paramList_.push_back(param);
}
