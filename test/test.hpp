/**
  * @file   Runtime.h 
  * @author sora
  * @date   2020/6/6
  */

#ifndef GEN_INTERFACE_V2_RUNTIME_RUNTIME_H_
#define GEN_INTERFACE_V2_RUNTIME_RUNTIME_H_
#include <variant>
#include <memory>
template<typename T>
class Proxy {
 private:
  using variant_type = std::variant<std::shared_ptr<T>, std::unique_ptr<T>, T *, std::pair<void *, void (*)(void *)>>;
  variant_type objPtr;
 public:
  ~Proxy() { if (objPtr.index() == 3) { std::get<3>(objPtr).second(std::get<3>(objPtr).first); }}
  T *get() const {
    switch (objPtr.index()) {
      case 0: { return std::get<0>(objPtr).get(); }
      case 1: { return std::get<1>(objPtr).get(); }
      case 2: { return std::get<2>(objPtr); }
      case 3: { return std::get<3>(objPtr).first; }
      default: throw std::runtime_error("invalid index");
    }
  }
  T *operator->() const {
    return this->get();
  }
  Proxy() : objPtr(nullptr) {}
  Proxy(nullptr_t) : objPtr(nullptr) {}
  Proxy(T *t) : objPtr(t) {}
  Proxy(T &t) : objPtr(&t) {}
  Proxy(T &&t) : objPtr(std::make_unique<T>(std::move(t))) {}
  Proxy(const std::shared_ptr<T> &t) : objPtr(t) {}
  Proxy(std::shared_ptr<T> &&t) : objPtr(std::move(t)) {}
  Proxy(const std::unique_ptr<T> &t) : objPtr(t.get()) {}
  Proxy(std::unique_ptr<T> &&t) : objPtr(std::move(t)) {}

  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(U *u) : objPtr(reinterpret_cast<T *>(u)) {}
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(U &u) : objPtr(reinterpret_cast<U *>(&u)) {}
  template<typename U,
           typename Urf = U &&,
           typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(Urf u) : objPtr(std::pair{new U(std::move(u)), [](void *ptr) { delete ((U *) ptr); }}) {}
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(const std::shared_ptr<U> &u) : objPtr(std::reinterpret_pointer_cast<T>(u)) {}
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(std::shared_ptr<U> &&u) : objPtr(std::reinterpret_pointer_cast<T>(std::move(u))) {}
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(const std::unique_ptr<U> &u): objPtr(reinterpret_cast<T>(u.get())) {}
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(std::unique_ptr<U> &&u) : objPtr(std::pair{u.release(), [](void *ptr) { delete ((U *) ptr); }}) {}

  Proxy(const Proxy &rhs) : objPtr(
      rhs.objPtr.index() == 0 ? variant_type(std::get<0>(rhs.objPtr)) : variant_type(rhs.get())) {}
  Proxy(Proxy &&rhs) : objPtr(std::move(rhs.objPtr)) {}
  Proxy &operator=(const Proxy &rhs) {
    if (&rhs == this) return *this;
    objPtr = rhs.objPtr.index() == 0 ? std::get<0>(rhs.objPtr) : rhs.get();
    return *this;
  }
  Proxy &operator=(Proxy &&rhs) {
    if (&rhs == this) return *this;
    objPtr = std::move(rhs.objPtr);
    rhs.objPtr = nullptr;
    return *this;
  }
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(const Proxy &u) {
    switch (u.objPtr.index()) {
      case 0: {
        objPtr = std::reinterpret_pointer_cast<T>(std::get<0>(u.objPtr));
        break;
      }
      case 1: {
        objPtr = reinterpret_cast<T *>(std::get<1>(u.objPtr).get());
        break;
      }
      case 2: {
        objPtr = reinterpret_cast<T *>(std::get<2>(u.objPtr));
        break;
      }
      case 3: {
        objPtr = reinterpret_cast<T *>(std::get<3>(u.objPtr).first);
        break;
      }
      default: throw std::runtime_error("invalid index");
    }
  }
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(U &&u) {
    switch (u.objPtr.index()) {
      case 0: {
        objPtr = std::reinterpret_pointer_cast<T>(std::move(std::get<0>(u.objPtr)));
        break;
      }
      case 1: {
        objPtr = std::pair{std::get<1>(u.objPtr).release(), [](void *ptr) { delete ((U *) ptr); }};
        break;
      }
      case 2: {
        objPtr = reinterpret_cast<T *>(std::get<2>(u.objPtr));
        break;
      }
      case 3: {
        objPtr = std::move(std::get<3>(u.objPtr));
        u.objPtr = nullptr;
        break;
      }
      default: throw std::runtime_error("invalid index");
    }
  }
};

#endif //GEN_INTERFACE_V2_RUNTIME_RUNTIME_H_

#ifndef __INTERFACE_____TEST_TEST_I
#define __INTERFACE_____TEST_TEST_I
#include <tuple>
#include <string>
#include <unordered_map>
#include <map>
//Comment
/*Comment
 * Comment
 */
#include "../example/interface.i"
template<typename T1, typename T2, typename T3>
class Test {
 private:
  template<typename __T>
  struct __VirtualTable {
    std::tuple<ssize_t, int> (__T::* read)(void *buf, size_t size);
    std::tuple<ssize_t, int> (__T::* write)(void *buf, size_t size);
    std::unordered_map<std::string, std::map<std::string, std::tuple<int, double, float>>> *(__T::* m1)(
        const std::tuple<std::string &, int *, const double *const> &t,
        const std::map<std::unordered_map<int, double>, std::string *> *m);
    inline static __VirtualTable virtualTable{.read=&__T::read, .write=&__T::write, .m1=&__T::m1,};
  };
  struct Dummy {};
 private:
  __VirtualTable<Dummy> *virtualTable;
  Proxy<Dummy> object;
 public:
  Test(void *vtb, Proxy<Dummy> &&obj) : virtualTable(reinterpret_cast<__VirtualTable<Dummy> *>(vtb),
                                                     object(std::move(obj)) {}
  template<typename __U>
  Test(__U *u) : virtualTable(reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)),
                 object(reinterpret_cast<Dummy *>(u)) {}
  template<typename __U>
  Test(__U &u) : virtualTable(reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)),
                 object(reinterpret_cast<Dummy *>(&u)) {}
  template<typename __U>
  Test(__U &&u) : virtualTable(reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)),
                  object(std::forward<U>(u)) {}
  template<typename __U>
  Test(const std::shared_ptr<__U> &u) : virtualTable(
      reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)), object(u) {}
  template<typename __U>
  Test(std::shared_ptr<__U> &&u) : virtualTable(
      reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)), object(std::move(u) {}
  template<typename __U>
  Test(const std::unique_ptr<__U> &u) : virtualTable(
      reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)), object(u) {}
  template<typename __U>
  Test(std::unique_ptr<__U> &&u) : virtualTable(
      reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)), object(std::move(u) {}
  std::tuple<ssize_t, int> read(void *buf, size_t size) {
    return ((object.get())->*(virtualTable->read))(std::forward<void *>(buf), std::forward<size_t>(size));
  }
  operator example::Reader() {
    return example::Reader(reinterpret_cast<void *>(reinterpret_cast<char *>(virtualTable) + sizeof(void *) * 0),
                           Proxy<example::Reader::Dummy>(*this->object)
  }
  operator example::Reader() && {
    return example::Reader(reinterpret_cast<void *>(reinterpret_cast<char *>(virtualTable) + sizeof(void *) * 0),
                           Proxy<example::Reader::Dummy>(std::move(*this->object))
  }
  std::tuple<ssize_t, int> write(void *buf, size_t size) {
    return ((object.get())->*(virtualTable->write))(std::forward<void *>(buf), std::forward<size_t>(size));
  }
  operator example::Writer() {
    return example::Writer(reinterpret_cast<void *>(reinterpret_cast<char *>(virtualTable) + sizeof(void *) * 1),
                           Proxy<example::Writer::Dummy>(*this->object)
  }
  operator example::Writer() && {
    return example::Writer(reinterpret_cast<void *>(reinterpret_cast<char *>(virtualTable) + sizeof(void *) * 1),
                           Proxy<example::Writer::Dummy>(std::move(*this->object))
  }
  operator ReadWriter() {
    return ReadWriter(reinterpret_cast<void *>(reinterpret_cast<char *>(virtualTable) + sizeof(void *) * 2),
                      Proxy<ReadWriter::Dummy>(*this->object)
  }
  operator ReadWriter() && {
    return ReadWriter(reinterpret_cast<void *>(reinterpret_cast<char *>(virtualTable) + sizeof(void *) * 2),
                      Proxy<ReadWriter::Dummy>(std::move(*this->object))
  }
  std::unordered_map<std::string, std::map<std::string, std::tuple<int, double, float>>> *m1(
      const std::tuple<std::string &, int *, const double *const> &t,
      const std::map<std::unordered_map<int, double>, std::string *> *m) {
    return ((object.get())->*(virtualTable->m1))(
        std::forward<const std::tuple<std::string &, int *, const double *const> &>(t),
        std::forward<const std::map<std::unordered_map<int, double>, std::string *> *>(m));
  }
};
class Test {
 private:
  template<typename __T>
  struct __VirtualTable {
    int (__T::* func2)();
    inline static __VirtualTable virtualTable{.func2=&__T::func2,};
  };
  struct Dummy {};
 private:
  __VirtualTable<Dummy> *virtualTable;
  Proxy<Dummy> object;
 public:
  Test(void *vtb, Proxy<Dummy> &&obj) : virtualTable(reinterpret_cast<__VirtualTable<Dummy> *>(vtb),
                                                     object(std::move(obj)) {}
  template<typename __U>
  Test(__U *u) : virtualTable(reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)),
                 object(reinterpret_cast<Dummy *>(u)) {}
  template<typename __U>
  Test(__U &u) : virtualTable(reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)),
                 object(reinterpret_cast<Dummy *>(&u)) {}
  template<typename __U>
  Test(__U &&u) : virtualTable(reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)),
                  object(std::forward<U>(u)) {}
  template<typename __U>
  Test(const std::shared_ptr<__U> &u) : virtualTable(
      reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)), object(u) {}
  template<typename __U>
  Test(std::shared_ptr<__U> &&u) : virtualTable(
      reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)), object(std::move(u) {}
  template<typename __U>
  Test(const std::unique_ptr<__U> &u) : virtualTable(
      reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)), object(u) {}
  template<typename __U>
  Test(std::unique_ptr<__U> &&u) : virtualTable(
      reinterpret_cast<__VirtualTable<Dummy> *>(&__VirtualTable<__U>::virtualTable)), object(std::move(u) {}
  int func2() {
    return ((object.get())->*(virtualTable->func2))();
  }
};
#endif
