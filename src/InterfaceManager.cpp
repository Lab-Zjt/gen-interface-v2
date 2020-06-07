/**
  * @file   InterfaceManager.cpp 
  * @author sora
  * @date   2020/6/5
  */

#include <fstream>
#include <iostream>
#include "InterfaceManager.h"
#include "Parser.h"

bool InterfaceManager::loadExternalInterface(const std::string &filename) {
  using namespace std;
  // load file
  ifstream ifs(filename);
  string importNamespace;
  Parser p(string{istreambuf_iterator<char>(ifs), istreambuf_iterator<char>()});
  // parse
  while (!p.finish()) {
    if (auto result = p.parse(); !result.success()) break;
    else {
      // set namespace
      if (result.isNamespace()) {
        if (result.getNamespace().namespace_ != curNamespace) { importNamespace = result.getNamespace().namespace_; }
      } else if (result.isImport()) {
        // recursively import
        auto &importName = result.getImport().import_;
        if (auto it = importName.find('"'); it == string::npos) {
          cerr << "invalid import name: " << importName << endl;
          return false;
        } else {
          if (auto pos = importName.find('"', it + 1); pos == string::npos) {
            cerr << "invalid import name: " << importName << endl;
            return false;
          } else {
            auto curDir = filename.substr(0, filename.rfind('/')) + "/";
            if (!loadExternalInterface(curDir + importName.substr(it + 1, pos - it - 1))) {
              cerr << "load import file: " << importName << " failed" << endl;
              return false;
            }
          }
        }
      } else if (result.isInterface()) {
        // add interface to externalInterfaceMap
        auto interfaceName = importNamespace.empty() ? result.getInterface().getName() : importNamespace + "::"
            + result.getInterface().getName();
        Interface i(result.getInterface());
        i.setName(interfaceName);
        externalInterfaceMap[interfaceName] = std::move(i);
      }
    }
  }
  if (!p.finish()) {
    cerr << "file parse terminate accidentally\n";
    return false;
  }
  return true;
}
extern char runtime_text[];
bool InterfaceManager::parse(const std::string &filename, std::ostream &os) {
  using namespace std;
  // print runtime file to os
  os << runtime_text << endl;
  // print protected define
  auto defineName = filename;
  for (auto it = 0ul; it != defineName.size(); ++it) {
    if (islower(defineName[it]))defineName[it] += 'A' - 'a';
    else if (!isalnum(defineName[it]) && defineName[it] != '_')defineName[it] = '_';
  }
  os << "#ifndef __INTERFACE__" << defineName << "\n";
  os << "#define __INTERFACE__" << defineName << "\n";
  // parse file
  vector<Parser::Result> resultList;
  ifstream ifs(filename);
  Parser p(string{istreambuf_iterator<char>(ifs), istreambuf_iterator<char>()});
  while (!p.finish()) {
    if (auto result = p.parse(); !result.success()) { break; }
    else { resultList.emplace_back(result); }
  }
  if (!p.finish()) {
    cerr << "parse " << filename << "failed\n";
    return false;
  }
  // visit result list
  for (auto &result : resultList) {
    if (result.isComment()) {
      // just print comment to os
      os << result.getComment().comment_ << endl;
    } else if (result.isNamespace()) {
      // set namespace
      if (!curNamespace.empty()) {
        cerr << "duplicate namespace: " << curNamespace << "|" << result.getNamespace().namespace_ << endl;
        return false;
      }
      curNamespace = result.getNamespace().namespace_;
      os << "namespace " << curNamespace << "{\n";
    } else if (result.isImport()) {
      // recursively import
      auto &importName = result.getImport().import_;
      if (auto it = importName.find('"'); it == string::npos) {
        cerr << "invalid import name: " << importName << endl;
        return false;
      } else {
        if (auto pos = importName.find('"', it + 1); pos == string::npos) {
          cerr << "invalid import name: " << importName << endl;
          return false;
        } else {
          auto curDir = filename.substr(0, filename.rfind('/')) + "/";
          if (!loadExternalInterface(curDir + importName.substr(it + 1, pos - it - 1))) return false;
        }
      }
      os << "#include " << result.getImport().import_.substr(0, result.getImport().import_.rfind('.')) + ".hpp" << '"'
         << endl;
    } else if (result.isInterface()) {
      // print interface
      printInterface(result.getInterface(), os);
    }
  }
  if (!curNamespace.empty())os << "}\n";
  os << "#endif\n";
  return true;
}

void InterfaceManager::printInterface(const Interface &i, std::ostream &os) {
  using namespace std;
  string className;
  // Class Name
  if (auto templateStart = i.getName().find('<'); templateStart != string::npos) {
    os << "template<";
    bool firstArg = true;
    for (auto it = templateStart + 1, argStart = it;; it++) {
      if (i.getName()[it] == ',' || i.getName()[it] == '>') {
        if (!firstArg) os << ",";
        else { firstArg = false; }
        os << "typename " << i.getName().substr(argStart, it - argStart);
        argStart = it + 1;
      }
      if (i.getName()[it] == '>') break;
    }
    os << ">\n";
    className = i.getName().substr(0, templateStart);
  } else {
    className = i.getName();
  }
  os << "class " << className << " ";
  // Body
  os << "{\n"
        " private:\n"
        "  template<typename __T>\n"
        // Virtual Table
        "  struct __VirtualTable {\n";
  vector<Interface> interfaceList;
  printMethodList(i, interfaceList, os);
  // initialize
  os << "    inline static __VirtualTable virtualTable{";
  for (auto &interface : interfaceList) {
    for (const auto &it : interface.getMethodList()) {
      os << /*"." << it->getName() << "=" */ "&__T::" << it.getName() << ",";
    }
  }
  os << "};\n"; // end initialize
  os << "  };\n"; // end virtual table
  // Dummy
  os << " public:\n"
        "  struct Dummy{};\n"
        // member
        " private:\n"
        "  __VirtualTable<Dummy>* virtualTable;\n"
        "  Proxy<Dummy> object;\n"
        " public:\n"
        "  "
        // constructor
     << className
     << "(void* vtb, Proxy<Dummy>&& obj) : "
        "virtualTable(reinterpret_cast<__VirtualTable<Dummy>*>(vtb)), "
        "object(std::move(obj)) {}\n"
     << "  template<typename __U, typename E = std::enable_if_t<!is_shared_ptr_v<__U> && !is_unique_ptr_v<__U>>>"
     << className
     << "(__U* u) : "
        "virtualTable(reinterpret_cast<__VirtualTable<Dummy>*>(&__VirtualTable<__U>::virtualTable)),"
        " object(reinterpret_cast<Dummy*>(u)) {}\n"
        "  template<typename __U, typename E = std::enable_if_t<!is_shared_ptr_v<__U> && !is_unique_ptr_v<__U>>>"
     << className
     << "(__U& u) : "
        "virtualTable(reinterpret_cast<__VirtualTable<Dummy>*>(&__VirtualTable<__U>::virtualTable)),"
        " object(reinterpret_cast<Dummy*>(&u)) {}\n"
        "  template<typename __U, typename E = std::enable_if_t<!is_shared_ptr_v<__U> && !is_unique_ptr_v<__U> && !std::is_lvalue_reference_v<__U>>>"
     << className
     << "(__U&& u) : "
        "virtualTable(reinterpret_cast<__VirtualTable<Dummy>*>(&__VirtualTable<__U>::virtualTable)),"
        " object(std::forward<__U>(u)) {}\n"
        "  template<typename __U>"
     << className
     << "(const std::shared_ptr<__U>& u) : "
        "virtualTable(reinterpret_cast<__VirtualTable<Dummy>*>(&__VirtualTable<__U>::virtualTable)),"
        " object(u) {}\n"
        "  template<typename __U>"
     << className
     << "(std::shared_ptr<__U>&& u) : "
        "virtualTable(reinterpret_cast<__VirtualTable<Dummy>*>(&__VirtualTable<__U>::virtualTable)),"
        " object(std::move(u)) {}\n"
        "  template<typename __U>"
     << className
     << "(const std::unique_ptr<__U>& u) : "
        "virtualTable(reinterpret_cast<__VirtualTable<Dummy>*>(&__VirtualTable<__U>::virtualTable)),"
        " object(u) {}\n"
        "  template<typename __U>"
     << className
     << "(std::unique_ptr<__U>&& u) : "
        "virtualTable(reinterpret_cast<__VirtualTable<Dummy>*>(&__VirtualTable<__U>::virtualTable)),"
        " object(std::move(u)) {}\n";
  // forward call
  std::size_t offset = 0;
  for (auto &interface : interfaceList) {
    for (auto &method : interface.getMethodList()) {
      os << "  " << method.getRet() << " " << method.getName() << "(";
      for (auto it = method.getParamList().begin(); it != method.getParamList().end(); ++it) {
        os << it->getType() << " " << it->getName();
        if (it + 1 != method.getParamList().end()) {
          os << ", ";
        }
      }
      os << ") {\n"
            "    return ((object.get())->*(virtualTable->"
         << method.getName()
         << "))(";
      for (auto it = method.getParamList().begin(); it != method.getParamList().end(); ++it) {
        os << "std::forward<" << it->getType() << ">(" << it->getName() << ")";
        if (it + 1 != method.getParamList().end()) {
          os << ", ";
        }
      }
      os << ");\n"
            "  }\n";
    }
    // conversion operator
    if (interface.getName().substr(0, className.size()) != className) {
      os << "  operator " << interface.getName() << "() & {\n"
         << "    return "
         << interface.getName()
         << "(reinterpret_cast<void*>(reinterpret_cast<char*>(virtualTable)+sizeof(void (Dummy::*)())*"
         << offset
         << "), "
         << "Proxy<"
         << interface.getName()
         << "::Dummy>(const_cast<const Proxy<Dummy>&>(object)));\n"
            "  }\n";
      os << "  operator " << interface.getName() << "() && {\n"
         << "    return "
         << interface.getName()
         << "(reinterpret_cast<void*>(reinterpret_cast<char*>(virtualTable)+sizeof(void (Dummy::*)())*"
         << offset * 2
         << "), "
         << "Proxy<"
         << interface.getName()
         << "::Dummy>(std::move(object)));\n"
            "  }\n";
      offset += interface.getMethodList().size();
    }
  }
  os << "};\n";
}
Interface *InterfaceManager::searchInterface(const std::string &name) {
  if (auto it = localInterfaceMap.find(name); it != localInterfaceMap.end()) {
    return &it->second;
  }
  if (auto it = localInterfaceMap.find(curNamespace + "::" + name); it != localInterfaceMap.end()) {
    return &it->second;
  }
  if (auto it = externalInterfaceMap.find(name); it != externalInterfaceMap.end()) {
    return &it->second;
  }
  if (auto it = externalInterfaceMap.find(curNamespace + "::" + name); it != externalInterfaceMap.end()) {
    return &it->second;
  }
  return nullptr;
}

void InterfaceManager::printMethodList(const Interface &i,
                                       std::vector<Interface> &interfaceList,
                                       std::ostream &os) {
  using namespace std;
  for (auto &extend : i.getExtendsList()) {
    if (auto baseInterface = searchInterface(extend); !baseInterface) {
      cerr << "base interface " << extend << " not found\n";
      return;
    } else {
      printMethodList(*baseInterface, interfaceList, os);
    }
  }
  for (auto &method : i.getMethodList()) {
    os << "    " << method.getRet() << "(__T::* " << method.getName() << ")";
    os << "(";
    for (auto it = method.getParamList().begin(); it != method.getParamList().end(); it++) {
      os << it->getType() << " " << it->getName();
      if (it + 1 != method.getParamList().end()) {
        os << ", ";
      }
    }
    os << ")" << method.getSpecifier() << ";\n";
  }
  interfaceList.push_back(i);
}