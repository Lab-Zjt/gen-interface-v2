/**
  * @file   Parser.cpp 
  * @author sora
  * @date   2020/6/5
  */

#include "Parser.h"
#include <iostream>
#include <stack>
using std::cout, std::cin, std::endl, std::cerr;
using std::optional, std::string, std::stack, std::nullopt;
#define ERR std::cerr << __FILE__ << ":" << __LINE__ << " [" << __func__ << "] " << '"' << currentContext() << '"' << ' '
auto trim(const std::string &str) -> std::string {
  if (auto start = str.find_first_not_of("\n\t\r\v "); start == std::string::npos) return "";
  else {
    if (auto end = str.find_last_not_of("\n\t\r\v "); end == std::string::npos) return "";
    else { return str.substr(start, end - start + 1); }
  }
}
Parser::Result Parser::parse() {
  auto nextCh = peek(2);
  if (!nextCh) {
    return {};
  }
  switch (nextCh->at(0)) {
    case '#':
    case '/': {
      if (auto r = parseComment(); r) { return *r; }
      else { return {}; }
    }
    case 'i': {
      switch (nextCh->at(1)) {
        case 'm': {
          if (auto r = parseImport(); r) { return *r; }
          else { return {}; }
        }
        case 'n': {
          if (auto r = parseInterface(); r) { return *r; }
          else { return {}; }
        }
        default: {
          return {};
        }
      }
    }
    case 'n': {
      if (auto r = parseNamespace(); r) { return *r; }
      else { return {}; }
    }
    default: {
      ERR << "unknown char detected: " << *nextCh << "\n";
      return {};
    }
  }
}

bool Parser::match(const std::string &sub) {
  if (ignoreEmptyChars()) {
    if (buf.find(sub, off) == off) {
      off += sub.size();
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool Parser::ignoreEmptyChars() {
  if (auto pos = buf.find_first_not_of("\r\n\t\v ", off); pos == std::string::npos) {
    off = buf.size();
    return false;
  } else {
    off = pos;
    return true;
  }
}

bool Parser::ignoreUntil(const std::string &terminator) {
  if (!ignoreEmptyChars()) { return {}; }
  if (auto pos = buf.find(terminator, off); pos == std::string::npos) {
    return false;
  } else {
    off = pos + terminator.size();
    return true;
  }
}

std::optional<std::string> Parser::readUntil(const std::string &terminator) {
  if (!ignoreEmptyChars()) { return {}; }
  if (auto pos = buf.find(terminator, off); pos == std::string::npos) {
    return {};
  } else {
    auto result = buf.substr(off, pos - off);
    off = pos + terminator.size();
    return result;
  }
}

std::optional<Import> Parser::parseImport() {
  if (!match("import")) {
    ERR << "require import\n";
    return nullopt;
  }
  if (auto i = readUntil(";"); !i) {
    ERR << "require ;";
    return nullopt;
  } else {
    return Import{trim(*i)};
  }
}

std::optional<Namespace> Parser::parseNamespace() {
  if (!match("namespace")) {
    ERR << "require namespace\n";
    return nullopt;
  }
  if (auto n = readUntil(";"); !n) {
    return nullopt;
  } else {
    return Namespace{trim(*n)};
  }
}

std::optional<Comment> Parser::parseComment() {
  if (match("//")) {
    return Comment{"//" + *readUntil("\n")};
  }
  if (match("#")) {
    return Comment{"#" + *readUntil("\n")};
  }
  if (match("/*")) {
    return Comment{"/*" + *readUntil("*/") + "*/"};
  }
  ERR << "require // or # or /*";
  return nullopt;
}

std::optional<Interface> Parser::parseInterface() {
  if (!match("interface")) { return nullopt; }
  if (auto id = readClassName(" {"); !id) { return nullopt; }
  else {
    Interface i;
    i.setName(trim(*id));
    if (match("extends")) {
      while (!match("{") && !finish()) {
        match(",");
        if (auto ext = readClassName(",{"); !ext) {
          ERR << "parse extends decl failed\n";
          return nullopt;
        } else {
          i.addExtends(trim(*ext));
        }
      }
    } else {
      if (!readUntil("{")) { return nullopt; };
    }
    while (!match("}")) {
      if (auto method = parseMethod(); method) {
        i.addMethod(*method);
      } else {
        return nullopt;
      }
    }
    return i;
  }
}

optional<std::pair<string, string>> Parser::parseTypeAndIdent(const string &deliSet) {
  stack<char> matchStack;
  auto it = off;
  do {
    switch (buf[it]) {
      case '<':
      case '[':
      case '{':
      case '(': {
        matchStack.push(buf[it]);
        break;
      }
      case '>' : {
        if (matchStack.top() != '<') {
          ERR << "bracket unbalance! found " << matchStack.top() << "but required <\n";
          return nullopt;
        } else { matchStack.pop(); }
        break;
      }

      case ']' : {
        if (matchStack.top() != '[') {
          ERR << "bracket unbalance! found " << matchStack.top() << "but required [\n";
          return nullopt;
        } else { matchStack.pop(); }
        break;
      }

      case '}' : {
        if (matchStack.top() != '{') {
          ERR << "bracket unbalance! found " << matchStack.top() << "but required }\n";
          return nullopt;
        } else { matchStack.pop(); }
        break;
      }

      case ')' : {
        if (matchStack.top() != '(') {
          ERR << "bracket unbalance! found " << matchStack.top() << "but required )\n";
          return nullopt;
        } else { matchStack.pop(); }
        break;
      }
      default:break;
    }
    it++;
  } while (!(matchStack.empty() && deliSet.find(buf[it]) != string::npos) && it < buf.size());
  if (it >= buf.size()) {
    ERR << "off out of range\n";
    return nullopt;
  }
  if (auto pos = buf.rfind(' ', it); pos == string::npos) {
    ERR << "can not split type and identifier\n";
    return nullopt;
  } else {
    auto type = buf.substr(off, pos - off);
    auto id = buf.substr(pos + 1, it - pos - 1);
    off = it;
    if (id == "operator") {
      if (off = buf.find('(', it + 1);off == string::npos) {
        ERR << "off out of range\n";
        return std::nullopt;
      }
      id += "()";
    }
    return std::pair{trim(type), trim(id)};
  }
}

std::optional<Method> Parser::parseMethod() {
  if (auto typeAndId = parseTypeAndIdent("("); !typeAndId) {
    ERR << "parse method failed.\n";
    return nullopt;
  } else {
    auto[retType, funcName] = std::move(*typeAndId);
    Method method;
    method.setRet(trim(retType));
    method.setName(trim(funcName));
    if (!match("(")) {
      ERR << "require (\n";
      return std::nullopt;
    }
    while (!match(")") && !finish()) {
      match(",");
      if (auto param = parseParam(); !param) {
        return std::nullopt;
      } else {
        method.addParam(*param);
      }
    }
    if (finish()) {
      ERR << "off out of range\n";
      return nullopt;
    }
    if (auto spec = readUntil(";"); !spec) {
      ERR << "parse specifier failed or require ;\n";
      return std::nullopt;
    } else {
      method.setSpecifier(trim(*spec));
    }
    return method;
  }
  return nullopt;
}
std::optional<std::string> Parser::peek(size_t size) {
  ignoreEmptyChars();
  if (finish() || size >= buf.size() - off) { return std::nullopt; }
  return buf.substr(off, size);
}
std::optional<Param> Parser::parseParam() {
  if (auto typeAndId = parseTypeAndIdent(",)"); !typeAndId) {
    ERR << "parse param failed.\n";
    return nullopt;
  } else {
    auto[type, id] = *typeAndId;
    Param p;
    p.setType(trim(type));
    p.setName(trim(id));
    return p;
  }
}
std::string Parser::currentContext(size_t len) {
  if (off < len) return buf.substr(0, len);
  else { return buf.substr(off - len, len); }
}
std::optional<std::string> Parser::readClassName(const std::string &deliSet) {
  stack<char> matchStack;
  auto it = off;
  do {
    switch (buf[it]) {
      case '<':
      case '[':
      case '{':
      case '(': {
        matchStack.push(buf[it]);
        break;
      }
      case '>' : {
        if (matchStack.top() != '<') {
          ERR << "bracket unbalance! found " << matchStack.top() << "but required <\n";
          return nullopt;
        } else { matchStack.pop(); }
        break;
      }

      case ']' : {
        if (matchStack.top() != '[') {
          ERR << "bracket unbalance! found " << matchStack.top() << "but required [\n";
          return nullopt;
        } else { matchStack.pop(); }
        break;
      }

      case '}' : {
        if (matchStack.top() != '{') {
          ERR << "bracket unbalance! found " << matchStack.top() << "but required }\n";
          return nullopt;
        } else { matchStack.pop(); }
        break;
      }

      case ')' : {
        if (matchStack.top() != '(') {
          ERR << "bracket unbalance! found " << matchStack.top() << "but required )\n";
          return nullopt;
        } else { matchStack.pop(); }
        break;
      }
      default:break;
    }
    it++;
  } while (!(matchStack.empty() && deliSet.find(buf[it]) != string::npos) && it < buf.size());
  if (it >= buf.size()) {
    ERR << "off out of range\n";
    return nullopt;
  }
  auto className = buf.substr(off, it - off);
  off = it;
  return trim(className);
}
