/**
  * @file   Param.h 
  * @author sora
  * @date   2020/6/5
  */

#ifndef GEN_INTERFACE_V2_SRC_PARAM_H_
#define GEN_INTERFACE_V2_SRC_PARAM_H_
#include <string>
// method param
class Param {
  std::string type_;
  std::string name_;
 public:
  const std::string &getType() const;
  void setType(const std::string &type);
  const std::string &getName() const;
  void setName(const std::string &name);
};

#endif //GEN_INTERFACE_V2_SRC_PARAM_H_
