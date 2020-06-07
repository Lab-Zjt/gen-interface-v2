/**
  * @file   Method.h 
  * @author sora
  * @date   2020/6/5
  */

#ifndef GEN_INTERFACE_V2_SRC_METHOD_H_
#define GEN_INTERFACE_V2_SRC_METHOD_H_
#include "Param.h"
#include <vector>

// Method Object
class Method {
  std::string name_;
  std::string ret_;
  std::vector<Param> paramList_;
  std::string specifier_;
 public:
  const std::string &getName() const;
  void setName(const std::string &name);
  const std::string &getRet() const;
  void setRet(const std::string &ret);
  const std::vector<Param> &getParamList() const;
  const std::string &getSpecifier() const { return specifier_; }
  void setSpecifier(const std::string &spec) { specifier_ = spec; }
  void addParam(const Param &param);
};

#endif //GEN_INTERFACE_V2_SRC_METHOD_H_
