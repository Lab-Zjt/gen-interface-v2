/**
  * @file   Parser.h 
  * @author sora
  * @date   2020/6/5
  */

#ifndef GEN_INTERFACE_V2_SRC_PARSER_H_
#define GEN_INTERFACE_V2_SRC_PARSER_H_
#include "Comment.h"
#include "Import.h"
#include "Interface.h"
#include "Method.h"
#include "Param.h"
#include "Namespace.h"
#include <optional>
#include <variant>
// Parser
class Parser {
 public:
  // Parse Result
  class Result {
   public:
    enum Type { COMMENT = 0, IMPORT = 1, INTERFACE = 2, METHOD = 3, PARAM = 4, NAMESPACE = 5 };
   private:
    std::optional<std::variant<Comment, Import, Interface, Method, Param, Namespace>> result_;
   public:
    Result() : result_(std::nullopt) {}
    Result(const Result &) = default;
    Result(Result &&) = default;
    template<typename T, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Result>>>
    Result(T &&t) : result_(std::forward<T>(t)) {}
    bool isComment() const { return result_ && result_->index() == COMMENT; }
    bool isImport() const { return result_ && result_->index() == IMPORT; }
    bool isInterface() const { return result_ && result_->index() == INTERFACE; }
    bool isMethod() const { return result_ && result_->index() == METHOD; }
    bool isParam() const { return result_ && result_->index() == PARAM; }
    bool isNamespace() const { return result_ && result_->index() == NAMESPACE; }
    size_t getIndex() const { return result_ ? -1 : result_->index(); }
    const Comment &getComment() const { return std::get<Comment>(*result_); }
    const Import &getImport() const { return std::get<Import>(*result_); }
    const Interface &getInterface() const { return std::get<Interface>(*result_); }
    const Method &getMethod() const { return std::get<Method>(*result_); }
    const Param &getParam() const { return std::get<Param>(*result_); }
    const Namespace &getNamespace() const { return std::get<Namespace>(*result_); }
    bool success() const { return result_.operator bool(); }
  };
 private:
  std::string buf;
  size_t off;
 private:
  // get context while parse failed
  std::string currentContext(size_t len = 32);
  // ignore any empty char
  bool ignoreEmptyChars();
  // read until terminator, set off to the position after terminator
  std::optional<std::string> readUntil(const std::string &terminator);
  // ignore until terminator, set off to the position after terminator
  bool ignoreUntil(const std::string &terminator);
  // check buf+off has prefix of sub, if success, set off to the position after sub, else do nothing
  bool match(const std::string &sub);
  // read a class name
  std::optional<std::string> readClassName(const std::string &deliSet);
  // peek a string with size on buf+off, will not move off
  std::optional<std::string> peek(size_t size);
  // parse a comment
  std::optional<Comment> parseComment();
  // parse a import
  std::optional<Import> parseImport();
  // parse a interface
  std::optional<Interface> parseInterface();
  // parse a method
  std::optional<Method> parseMethod();
  // parse a param
  std::optional<Param> parseParam();
  // parse a namespace
  std::optional<Namespace> parseNamespace();
  // parse a type and an identifier (when parse method return type and name, param type and name)
  std::optional<std::pair<std::string, std::string>> parseTypeAndIdent(const std::string &deliSet);
 public:
  Parser(std::string str) : buf(std::move(str)), off(0) {}
  Result parse();
  bool finish() { return off >= buf.size(); }
};

#endif //GEN_INTERFACE_V2_SRC_PARSER_H_
