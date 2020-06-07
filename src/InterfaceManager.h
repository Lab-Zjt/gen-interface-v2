/**
  * @file   InterfaceManager.h 
  * @author sora
  * @date   2020/6/5
  */

#ifndef GEN_INTERFACE_V2_SRC_INTERFACEMANAGER_H_
#define GEN_INTERFACE_V2_SRC_INTERFACEMANAGER_H_
#include "Interface.h"
#include <unordered_map>
// parse interface file and output the result
class InterfaceManager {
  std::unordered_map<std::string, Interface> externalInterfaceMap;
  std::unordered_map<std::string, Interface> localInterfaceMap;
  std::string curNamespace;
  // recursively load <import file> into externalInterfaceMap
  bool loadExternalInterface(const std::string &filename);
  // searchInterface on externalInterfaceMap and localInterfaceMap
  Interface *searchInterface(const std::string &name);
  // print interface to os
  void printInterface(const Interface &i, std::ostream& os);
  // print method list to os recursively, save all interface(include base interface) to interfaceList
  void printMethodList(const Interface &i, std::vector<Interface> &interfaceList,
                       std::ostream &os);
 public:
  // parse a file and print result to os
  bool parse(const std::string &filename, std::ostream &os = std::cout);
};

#endif //GEN_INTERFACE_V2_SRC_INTERFACEMANAGER_H_
