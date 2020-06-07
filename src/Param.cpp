/**
  * @file   Param.cpp 
  * @author sora
  * @date   2020/6/5
  */

#include "Param.h"
const std::string &Param::getType() const {
  return type_;
}
void Param::setType(const std::string &type) {
  type_ = type;
}
const std::string &Param::getName() const {
  return name_;
}
void Param::setName(const std::string &name) {
  name_ = name;
}
